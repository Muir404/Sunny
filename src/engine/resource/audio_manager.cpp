// 实现文件
#include "audio_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{

    // ========================== 构造/析构函数 ==========================
    AudioManager::AudioManager()
    {
        // 初始化 SDL_mixer 子系统
        if (!MIX_Init())
        {
            throw std::runtime_error("AudioManager 错误: MIX_Init 失败: " + std::string(SDL_GetError()));
        }

        mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (!mixer_)
        {
            MIX_Quit();
            throw std::runtime_error("AudioManager 错误:Mixer创建失败" + std::string(SDL_GetError()));
        }

        spdlog::trace("[AudioManager] 构造成功");
    }

    AudioManager::~AudioManager()
    {
        // 暂停音频
        MIX_StopAllTracks(mixer_, 0);

        // 清理所有音频资源
        clearAudio();

        // 销毁Mixer
        if (mixer_ != nullptr)
        {
            MIX_DestroyMixer(mixer_);
            mixer_ = nullptr; // 置空，避免野指针
            spdlog::trace("[AudioManager] mixer销毁成功");
        }
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

    // ========================== 资源清理（顶层） ==========================
    void AudioManager::clearAudio()
    {
        clearSounds(); // 清理音效
        clearMusic();  // 清理音乐
        spdlog::debug("[AudioManager] 所有音频资源已清空");
    }

} // namespace engine::resource
