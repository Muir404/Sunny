// 实现文件
#include "audio_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{

    // ========================== 构造/析构函数 ==========================
    AudioManager::AudioManager()
    {
        // 初始化 SDL_mixer 子系统（SDL3_mixer 无需传入格式标志）
        if (!MIX_Init())
        {
            throw std::runtime_error("AudioManager 错误: MIX_Init 失败: " + std::string(SDL_GetError()));
        }

        // 创建混音器设备（SDL3 新接口，传入默认播放设备和空格式（由系统自动选择最优格式））
        mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!mixer_)
        {
            MIX_Quit(); // 初始化失败，清理已初始化的子系统
            throw std::runtime_error("AudioManager 错误: 打开音频设备失败: " + std::string(SDL_GetError()));
        }

        // 初始化轨道对象池
        createTrackPool(DEFAULT_TRACK_POOL_SIZE);
        spdlog::info("[AudioManager] 轨道对象池创建完成，容量: {}", DEFAULT_TRACK_POOL_SIZE);

        spdlog::trace("[AudioManager] 构造成功");
    }

    AudioManager::~AudioManager()
    {
        // 立即停止所有轨道播放（SDL_mixer 3.0 中 MIX_StopAllTracks 第二个参数为毫秒）
        if (mixer_)
        {
            MIX_StopAllTracks(mixer_, 0); // 0ms 立即停止
            spdlog::trace("[AudioManager] 已停止所有音频轨道播放");
        }

        // 清理所有音频资源
        clearAudio();

        // 退出 SDL_mixer 子系统
        MIX_Quit();

        spdlog::trace("[AudioManager] 析构完成");
    }

    // ========================== 音效管理（Sound） ==========================
    MIX_Audio *AudioManager::loadSound(const std::string &file_path)
    {
        // 检查缓存，避免重复加载
        // 负责实际从文件加载音效，并存入缓存
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            spdlog::trace("[AudioManager] 音效已缓存，直接返回: {}", file_path);
            return it->second.get();
        }

        // 加载音效文件（predecode=false：不预解码，播放时实时解码，节省内存）
        spdlog::debug("[AudioManager] 开始加载音效: {}", file_path);
        MIX_Audio *audio = MIX_LoadAudio(mixer_, file_path.c_str(), false);
        if (!audio)
        {
            spdlog::error("[AudioManager] 加载音效失败: {} | 错误信息: {}", file_path, SDL_GetError());
            return nullptr;
        }

        // 存入缓存（使用自定义删除器管理生命周期）
        sounds_.emplace(file_path, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(audio));
        spdlog::debug("[AudioManager] 音效加载并缓存成功: {}", file_path);

        return audio; // 返回一个音频载体，待绑定播放
    }

    MIX_Audio *AudioManager::getSound(const std::string &file_path)
    {
        // 优先从缓存获取
        // 负责对外提供音效，优先查缓存，无则调用加载器
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            return it->second.get();
        }
        spdlog::warn("[AudioManager] 音效缓存未命中，尝试动态加载: {}", file_path);
        return loadSound(file_path);
    }

    void AudioManager::unloadSound(const std::string &file_path)
    {
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            spdlog::debug("[AudioManager] 卸载音效资源: {}", file_path);
            sounds_.erase(it); // unique_ptr 自动释放资源
        }
        else
        {
            spdlog::warn("[AudioManager] 尝试卸载不存在的音效资源: {}", file_path);
        }
    }

    void AudioManager::clearSounds()
    {
        if (!sounds_.empty())
        {
            spdlog::debug("[AudioManager] 清空所有音效缓存，数量: {}", sounds_.size());
            sounds_.clear(); // unique_ptr 自动释放所有音效资源
        }
    }

    // ========================== 音乐管理（Music） ==========================
    MIX_Audio *AudioManager::loadMusic(const std::string &file_path)
    {
        // 检查缓存，避免重复加载
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            spdlog::trace("[AudioManager] 音乐已缓存，直接返回: {}", file_path);
            return it->second.get();
        }

        // 加载音乐文件（predecode=true：预解码为 PCM 数据，适合长音频流式播放）
        spdlog::debug("[AudioManager] 开始加载音乐: {}", file_path);
        MIX_Audio *audio = MIX_LoadAudio(mixer_, file_path.c_str(), true);
        if (!audio)
        {
            spdlog::error("[AudioManager] 加载音乐失败: {} | 错误信息: {}", file_path, SDL_GetError());
            return nullptr;
        }

        // 存入缓存（使用自定义删除器管理生命周期）
        music_.emplace(file_path, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(audio));
        spdlog::debug("[AudioManager] 音乐加载并缓存成功: {}", file_path);

        return audio;
    }

    MIX_Audio *AudioManager::getMusic(const std::string &file_path)
    {
        // 优先从缓存获取
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            return it->second.get();
        }

        // 缓存未命中，尝试加载
        spdlog::warn("[AudioManager] 音乐缓存未命中，尝试动态加载: {}", file_path);
        return loadMusic(file_path);
    }

    void AudioManager::unloadMusic(const std::string &file_path)
    {
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            spdlog::debug("[AudioManager] 卸载音乐资源: {}", file_path);
            music_.erase(it); // unique_ptr 自动释放资源
        }
        else
        {
            spdlog::warn("[AudioManager] 尝试卸载不存在的音乐资源: {}", file_path);
        }
    }

    void AudioManager::clearMusic()
    {
        if (!music_.empty())
        {
            spdlog::debug("[AudioManager] 清空所有音乐缓存，数量: {}", music_.size());
            music_.clear(); // unique_ptr 自动释放所有音乐资源
        }
    }

    // ========================== 命名轨道管理（NamedTrack） ==========================
    MIX_Track *AudioManager::loadNamedTrack(const std::string &track_name)
    {
        // 检查缓存，避免重复创建（和 loadSound/loadMusic 逻辑完全一致）
        auto it = named_tracks_.find(track_name);
        if (it != named_tracks_.end())
        {
            spdlog::trace("[AudioManager] 命名轨道已缓存，直接返回: {}", track_name);
            return it->second.get();
        }

        // 创建新的命名轨道（对应 Sound/Music 的 "加载文件" 逻辑）
        spdlog::debug("[AudioManager] 开始加载命名轨道: {}", track_name);
        MIX_Track *track = MIX_CreateTrack(mixer_);
        if (!track)
        {
            spdlog::error("[AudioManager] 加载命名轨道失败: {} | 错误信息: {}", track_name, SDL_GetError());
            return nullptr;
        }

        // 存入缓存（使用自定义删除器管理生命周期）
        named_tracks_.emplace(track_name, std::unique_ptr<MIX_Track, TrackDeleter>(track));
        spdlog::debug("[AudioManager] 命名轨道加载并缓存成功: {}", track_name);

        return track;
    }

    MIX_Track *AudioManager::getNamedTrack(const std::string &track_name)
    {
        // 优先从缓存获取（和 getSound/getMusic 逻辑完全一致）
        auto it = named_tracks_.find(track_name);
        if (it != named_tracks_.end())
        {
            return it->second.get();
        }

        // 缓存未命中，尝试动态加载
        spdlog::warn("[AudioManager] 命名轨道缓存未命中，尝试动态加载: {}", track_name);
        return loadNamedTrack(track_name);
    }

    void AudioManager::unloadNamedTrack(const std::string &track_name)
    {
        auto it = named_tracks_.find(track_name);
        if (it != named_tracks_.end())
        {
            // 停止轨道播放并解绑音频（轨道特有清理逻辑）
            MIX_Track *track = it->second.get();
            if (track)
            {
                MIX_StopTrack(track, 0);
                MIX_SetTrackAudio(track, nullptr);
            }

            spdlog::debug("[AudioManager] 卸载命名轨道资源: {}", track_name);
            named_tracks_.erase(it); // unique_ptr 自动释放资源
        }
        else
        {
            spdlog::warn("[AudioManager] 尝试卸载不存在的命名轨道资源: {}", track_name);
        }
    }

    void AudioManager::clearNamedTracks()
    {
        if (!named_tracks_.empty())
        {
            // 先停止所有命名轨道播放
            for (auto &[name, track_ptr] : named_tracks_)
            {
                if (track_ptr)
                {
                    MIX_StopTrack(track_ptr.get(), 0);
                    MIX_SetTrackAudio(track_ptr.get(), nullptr);
                }
            }

            spdlog::debug("[AudioManager] 清空所有命名轨道缓存，数量: {}", named_tracks_.size());
            named_tracks_.clear(); // unique_ptr 自动释放所有资源
        }
    }

    // ========================== 临时轨道池管理（TemporaryTrack） ==========================
    MIX_Track *AudioManager::getTemporaryTrack()
    {
        // 池为空时创建新轨道（加入智能指针管理，避免内存泄漏）
        if (track_pool_.empty())
        {
            spdlog::warn("[AudioManager] 轨道对象池为空，创建新临时轨道");

            MIX_Track *newTrack = MIX_CreateTrack(mixer_);
            if (!newTrack)
            {
                spdlog::error("[AudioManager] 创建临时轨道失败 | 错误信息: {}", SDL_GetError());
                return nullptr;
            }

            // 优化：直接创建智能指针并释放所有权，无需入池再取出
            std::unique_ptr<MIX_Track, TrackDeleter> trackPtr(newTrack);
            spdlog::debug("[AudioManager] 临时轨道创建成功（池扩容）");
            return trackPtr.release();
        }

        // 从池中取出最后一个轨道（效率更高）
        auto track = std::move(track_pool_.back());
        track_pool_.pop_back();

        spdlog::trace("[AudioManager] 从对象池获取临时轨道，剩余轨道数: {}", track_pool_.size());

        // 释放所有权给调用者（调用者使用完需调用 releaseTemporaryTrack 归还）
        return track.release();
    }

    void AudioManager::releaseTemporaryTrack(MIX_Track *track)
    {
        if (!track)
        {
            spdlog::warn("[AudioManager] 尝试归还空的临时轨道指针，忽略");
            return;
        }

        // 补充：检查轨道是否已在池中（防止重复归还导致双重释放）
        for (const auto &poolTrack : track_pool_)
        {
            if (poolTrack.get() == track)
            {
                spdlog::warn("[AudioManager] 轨道已在池中，无需重复归还");
                return;
            }
        }

        // 停止轨道播放（SDL_mixer 3.0 中 MIX_StopTrack 第二个参数为采样帧，0 表示立即停止）
        MIX_StopTrack(track, 0);
        // 清空轨道关联的音频资源
        MIX_SetTrackAudio(track, nullptr);

        // 检查轨道是否属于当前混音器（避免野指针）
        if (MIX_GetTrackMixer(track) != mixer_)
        {
            spdlog::error("[AudioManager] 尝试归还不属于当前混音器的临时轨道，直接销毁");
            MIX_DestroyTrack(track);
            return;
        }

        // 归还到对象池（智能指针接管生命周期）
        track_pool_.emplace_back(track);
        spdlog::trace("[AudioManager] 临时轨道归还到对象池，当前池容量: {}", track_pool_.size());
    }

    void AudioManager::createTrackPool(size_t poolSize)
    {
        // 预分配内存，避免频繁扩容
        track_pool_.reserve(poolSize);
        size_t createdCount = 0;

        // 创建指定数量的轨道
        for (size_t i = 0; i < poolSize; ++i)
        {
            MIX_Track *track = MIX_CreateTrack(mixer_);
            if (track)
            {
                track_pool_.emplace_back(track);
                createdCount++;
            }
            else
            {
                spdlog::error("[AudioManager] 创建轨道对象池失败 | 第 {} 个轨道创建失败 | 错误信息: {}",
                              i + 1, SDL_GetError());
            }
        }

        spdlog::debug("[AudioManager] 轨道对象池初始化完成 | 目标容量: {} | 实际创建: {}",
                      poolSize, createdCount);
    }

    void AudioManager::clearTrackPool()
    {
        if (!track_pool_.empty())
        {
            spdlog::debug("[AudioManager] 清空临时轨道池，数量: {}", track_pool_.size());
            track_pool_.clear();
        }
    }

    // ========================== 分组管理（Group） ==========================
    MIX_Group *AudioManager::loadGroup(const std::string &group_name)
    {
        // 检查缓存，避免重复创建（和 loadSound/loadMusic 逻辑完全一致）
        auto it = audio_groups_.find(group_name);
        if (it != audio_groups_.end())
        {
            spdlog::trace("[AudioManager] 音频分组已缓存，直接返回: {}", group_name);
            return it->second.get();
        }

        // 创建新分组（对应 Sound/Music 的 "加载文件" 逻辑）
        spdlog::debug("[AudioManager] 开始加载音频分组: {}", group_name);
        MIX_Group *group = MIX_CreateGroup(mixer_);
        if (!group)
        {
            spdlog::error("[AudioManager] 加载音频分组失败: {} | 错误信息: {}", group_name, SDL_GetError());
            return nullptr;
        }

        // 存入缓存（使用自定义删除器管理生命周期）
        audio_groups_.emplace(group_name, std::unique_ptr<MIX_Group, GroupDeleter>(group));
        spdlog::debug("[AudioManager] 音频分组加载并缓存成功: {}", group_name);

        return group;
    }

    MIX_Group *AudioManager::getGroup(const std::string &group_name)
    {
        // 优先从缓存获取（和 getSound/getMusic 逻辑完全一致）
        auto it = audio_groups_.find(group_name);
        if (it != audio_groups_.end())
        {
            return it->second.get();
        }

        // 缓存未命中，尝试动态加载
        spdlog::warn("[AudioManager] 音频分组缓存未命中，尝试动态加载: {}", group_name);
        return loadGroup(group_name);
    }

    void AudioManager::unloadGroup(const std::string &group_name)
    {
        auto it = audio_groups_.find(group_name);
        if (it != audio_groups_.end())
        {
            spdlog::debug("[AudioManager] 卸载音频分组资源: {}", group_name);
            audio_groups_.erase(it); // unique_ptr 自动释放资源
        }
        else
        {
            spdlog::warn("[AudioManager] 尝试卸载不存在的音频分组资源: {}", group_name);
        }
    }

    void AudioManager::clearGroups()
    {
        if (!audio_groups_.empty())
        {
            spdlog::debug("[AudioManager] 清空所有音频分组缓存，数量: {}", audio_groups_.size());
            audio_groups_.clear(); // unique_ptr 自动释放所有资源
        }
    }

    bool AudioManager::addTrackToGroup(MIX_Track *track, MIX_Group *group)
    {
        // 参数合法性检查
        if (!track || !group)
        {
            spdlog::warn("[AudioManager] 轨道加入分组失败 | 轨道/分组指针为空");
            return false;
        }

        // 将轨道加入分组（SDL3_mixer 中 bool 型 API 返回 SDL_TRUE/SDL_FALSE）
        if (!MIX_SetTrackGroup(track, group))
        {
            spdlog::error("[AudioManager] 轨道加入分组失败 | 错误信息: {}", SDL_GetError());
            return false;
        }

        spdlog::trace("[AudioManager] 轨道成功加入分组");
        return true;
    }

    // ========================== 轨道标签（Tag） ==========================
    bool AudioManager::tagTrack(MIX_Track *track, const std::string &tag)
    {
        // 参数合法性检查
        if (!track)
        {
            spdlog::warn("[AudioManager] 为轨道添加标签失败 | 轨道指针为空");
            return false;
        }

        // 添加标签（SDL3_mixer 中 MIX_TagTrack 返回 bool）
        if (!MIX_TagTrack(track, tag.c_str()))
        {
            spdlog::error("[AudioManager] 为轨道添加标签失败: {} | 错误信息: {}", tag, SDL_GetError());
            return false;
        }

        spdlog::trace("[AudioManager] 轨道标签添加成功: {}", tag);
        return true;
    }

    bool AudioManager::untagTrack(MIX_Track *track, const std::string &tag)
    {
        // 参数合法性检查
        if (!track)
        {
            spdlog::warn("[AudioManager] 移除轨道标签失败 | 轨道指针为空");
            return false;
        }

        // 移除标签（MIX_UntagTrack 无返回值，SDL3_mixer 中该函数总是成功）
        MIX_UntagTrack(track, tag.c_str());
        spdlog::trace("[AudioManager] 轨道标签移除成功: {}", tag);

        return true;
    }

    // ========================== 混音器控制（Mixer） ==========================
    bool AudioManager::setMasterGain(float gain)
    {
        // 混音器未初始化
        if (!mixer_)
        {
            spdlog::warn("[AudioManager] 设置主音量失败 | 混音器未初始化");
            return false;
        }

        // 校验音量范围（SDL3_mixer 中 gain 可以超过 1.0，这里限制为 0~1 是业务逻辑）
        if (gain < 0.0f || gain > 1.0f)
        {
            spdlog::warn("[AudioManager] 设置主音量失败 | 音量值超出范围 [0.0, 1.0]: {}", gain);
            return false;
        }

        // 设置主音量（SDL3_mixer 中 MIX_SetMixerGain 返回 bool）
        if (!MIX_SetMixerGain(mixer_, gain))
        {
            spdlog::error("[AudioManager] 设置主音量失败 | 目标值: {:.2f} | 错误信息: {}", gain, SDL_GetError());
            return false;
        }

        spdlog::trace("[AudioManager] 主音量设置成功: {:.2f}", gain);
        return true;
    }

    bool AudioManager::setTrackGain(MIX_Track *track, float gain)
    {
        if (track == nullptr)
        {
            spdlog::warn("[AudioManager] 设置音量失败");
            return false;
        }
        if (gain < 0.0f || gain > 1.0f)
        {
            spdlog::warn("[AudioManager] 设置音量失败 | 音量值超出范围 [0.0, 1.0]: {}", gain);
            return false;
        }
        // 设置音量
        if (!MIX_SetTrackGain(track, gain))
        {
            spdlog::error("[AudioManager] 设置音量失败 | 目标值: {:.2f} | 错误信息: {}", gain, SDL_GetError());
            return false;
        }

        spdlog::trace("[AudioManager] 音量设置成功: {:.2f}", gain);
        return true;
    }

    void AudioManager::clearMixer()
    {
        if (mixer_)
        {
            MIX_DestroyMixer(mixer_);
            mixer_ = nullptr;
            spdlog::trace("[AudioManager] 混音器资源已清理");
        }
    }

    // ========================== 资源清理（顶层） ==========================
    void AudioManager::clearAudio()
    {
        // 1. 停止所有轨道播放
        if (mixer_)
        {
            MIX_StopAllTracks(mixer_, 0);
        }

        // 2.1 清理音效
        clearSounds();
        // 2.2 清理音乐
        clearMusic();
        // 3. 清理命名轨道（使用新接口）
        clearNamedTracks();
        // 4. 清理临时轨道池
        clearTrackPool();
        // 5. 清理音频分组
        clearGroups();
        // 6. 清理混音器
        clearMixer();

        spdlog::debug("[AudioManager] 所有音频资源已清空");
    }

} // namespace engine::resource
