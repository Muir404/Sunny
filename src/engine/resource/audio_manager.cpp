#include "audio_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace engine::resource
{
    AudioManager::AudioManager()
    {
        // 步骤 1初始化 SDL3_mixer 库
        if (!MIX_Init())
        {
            throw std::runtime_error("音频管理器错误: MIX_Init 失败: " + std::string(SDL_GetError()));
        }

        // 步骤 2：创建设备混音器（保存到类成员变量 m_mixer，长期持有）
        m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (m_mixer == NULL)
        {
            MIX_Quit();
            throw std::runtime_error("音频管理器错误: 混音器创建失败（等效 Mix_OpenAudio 失败）: " + std::string(SDL_GetError()));
        }

        spdlog::trace("AudioManager 构造成功。");
    }

    AudioManager::~AudioManager()
    {

        if (m_mixer != nullptr)
        {
            MIX_StopAllTracks(m_mixer, 0); // 0 表示立即停止，无淡出效果
            clearAudio();
            MIX_DestroyMixer(m_mixer);
            m_mixer = nullptr; // 置空避免悬空指针
        }

        // 步骤 4：退出 SDL3_mixer 库
        MIX_Quit();

        spdlog::trace("音频管理器析构成功。");
    }

    // --- 音效管理（对应原有 Mix_Chunk 逻辑，预解码到内存）---
    MIX_Audio *AudioManager::loadSound(const std::string &file_path)
    {
        // 1. 缓存优先检查（与原有逻辑一致）
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            return it->second.get();
        }

        // 2. 加载音效（SDL3_mixer 替换 Mix_LoadWAV()，预解码=true 对应 Mix_Chunk 特性）
        spdlog::debug("加载音效: {}", file_path);
        MIX_Audio *raw_audio = MIX_LoadAudio(m_mixer, file_path.c_str(), true);
        if (!raw_audio)
        {
            spdlog::error("加载音效失败: '{}': {}", file_path, SDL_GetError());
            return nullptr;
        }

        // 3. 存入缓存（unique_ptr 管理，自定义删除器自动释放）
        sounds_.emplace(file_path, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(raw_audio));
        spdlog::debug("成功加载并缓存音效: {}", file_path);
        return raw_audio;
    }

    MIX_Audio *AudioManager::getSound(const std::string &file_path)
    {
        // 1. 先查缓存（与原有逻辑一致）
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            return it->second.get();
        }

        // 2. 缓存缺失，尝试懒加载（与原有逻辑一致）
        spdlog::warn("音效 '{}' 未找到缓存，尝试加载。", file_path);
        return loadSound(file_path);
    }

    void AudioManager::unloadSound(const std::string &file_path)
    {
        // 1. 查找并删除指定音效（与原有逻辑一致，unique_ptr 自动调用 MIX_DestroyAudio()）
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            spdlog::debug("卸载音效: {}", file_path);
            sounds_.erase(it);
        }
        else
        {
            spdlog::warn("尝试卸载不存在的音效: {}", file_path);
        }
    }

    void AudioManager::clearSounds()
    {
        // 1. 批量清理所有音效（与原有逻辑一致，map 清空时 unique_ptr 自动释放资源）
        if (!sounds_.empty())
        {
            spdlog::debug("正在清除所有 {} 个缓存的音效。", sounds_.size());
            sounds_.clear();
        }
    }

    // --- 音乐管理（对应原有 Mix_Music 逻辑，流式实时解码）---
    MIX_Audio *AudioManager::loadMusic(const std::string &file_path)
    {
        // 1. 缓存优先检查（与原有逻辑一致）
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            return it->second.get();
        }

        // 2. 加载音乐（SDL3_mixer 替换 Mix_LoadMUS()，预解码=false 对应 Mix_Music 流式特性）
        spdlog::debug("加载音乐: {}", file_path);
        MIX_Audio *raw_audio = MIX_LoadAudio(m_mixer, file_path.c_str(), false);
        if (!raw_audio)
        {
            spdlog::error("加载音乐失败: '{}': {}", file_path, SDL_GetError());
            return nullptr;
        }

        // 3. 存入缓存（unique_ptr 管理，自定义删除器自动释放）
        music_.emplace(file_path, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(raw_audio));
        spdlog::debug("成功加载并缓存音乐: {}", file_path);
        return raw_audio;
    }

    MIX_Audio *AudioManager::getMusic(const std::string &file_path)
    {
        // 1. 先查缓存（与原有逻辑一致）
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            return it->second.get();
        }

        // 2. 缓存缺失，尝试懒加载（与原有逻辑一致）
        spdlog::warn("音乐 '{}' 未找到缓存，尝试加载。", file_path);
        return loadMusic(file_path);
    }

    void AudioManager::unloadMusic(const std::string &file_path)
    {
        // 1. 查找并删除指定音乐（与原有逻辑一致，unique_ptr 自动调用 MIX_DestroyAudio()）
        auto it = music_.find(file_path);
        if (it != music_.end())
        {
            spdlog::debug("卸载音乐: {}", file_path);
            music_.erase(it);
        }
        else
        {
            spdlog::warn("尝试卸载不存在的音乐: {}", file_path);
        }
    }

    void AudioManager::clearMusic()
    {
        // 1. 批量清理所有音乐（与原有逻辑一致，map 清空时 unique_ptr 自动释放资源）
        if (!music_.empty())
        {
            spdlog::debug("正在清除所有 {} 个缓存的音乐曲目。", music_.size());
            music_.clear();
        }
    }

    // --- 全量音频清理 ---
    void AudioManager::clearAudio()
    {
        // 保留原有逻辑，先清音效，再清音乐

        clearMusic();
        clearSounds();
        spdlog::debug("所有音频资源清理完成。");
    }
} // namespace engine::resource // 修正拼写错误：resouce → resource
