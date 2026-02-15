#pragma once

// 标准库头文件（按字母序排列）
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// 第三方库头文件
#include <SDL3_mixer/SDL_mixer.h>

namespace engine::resource
{

    /**
     * @brief 管理 SDL_mixer 3.0 音频资源
     *
     * 提供音频资源的加载、缓存、轨道管理和分组控制功能
     * 构造失败时会抛出异常，仅供 ResourceManager 内部使用
     */
    class AudioManager final
    {
        // 友元声明
        friend class ResourceManager;

    private:
        /**
         * @brief MIX_Audio 自定义删除器
         * SDL_mixer 3.0 中音效/音乐统一为 MIX_Audio，无需区分 Chunk/Music
         */
        struct SDLMixAudioDeleter
        {
            void operator()(MIX_Audio *audio) const
            {
                if (audio)
                {
                    MIX_DestroyAudio(audio);
                }
            }
        };

        // struct SDLMixMixerDeleter
        // {
        //     void operator()(MIX_Mixer *mixer) const
        //     {
        //         if (mixer)
        //         {
        //             MIX_DestroyMixer(mixer);
        //         }
        //     }
        // };

    private:
        // 资源缓存
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> sounds_; // 短音效缓存 (路径 -> 音频资源)
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> music_;  // 长音乐缓存 (路径 -> 音频资源)
        // std::unique_ptr<MIX_Mixer, SDLMixMixerDeleter> mixer_;
        MIX_Mixer *mixer_;

        // ========================== 公有接口（Public Interface）==========================
    public:
        /**
         * @brief 构造函数
         * 初始化 SDL_mixer 并打开音频设备，预分配轨道池
         * @throws std::runtime_error SDL_mixer 初始化或音频设备打开失败时抛出
         */
        AudioManager();

        /**
         * @brief 析构函数
         * 清理所有音频资源，关闭 SDL_mixer 并释放设备
         */
        ~AudioManager();

        // 禁用拷贝和移动语义（单例语义）
        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;

        // ========================== 私有接口（Private Interface）==========================
    private:
        // getter and setter
        MIX_Mixer *getMixer()
        {
            return mixer_;
        }

        // -------------------------- 资源加载/卸载 --------------------------
        /**
         * @brief 加载音效资源（短音频，预解码到内存）
         * @param file_path 音效文件路径
         * @return 加载成功返回 MIX_Audio 指针，失败返回 nullptr
         */
        MIX_Audio *loadSound(std::string_view file_path);

        /**
         * @brief 获取音效资源（缓存优先）
         * @param file_path 音效文件路径
         * @return 成功返回 MIX_Audio 指针，失败抛出异常
         */
        MIX_Audio *getSound(std::string_view file_path);

        /**
         * @brief 卸载指定音效资源
         * @param file_path 音效文件路径
         */
        void unloadSound(std::string_view file_path);

        /**
         * @brief 清空所有音效资源
         */
        void clearSounds();

        /**
         * @brief 加载音乐资源（长音频，流式解码）
         * @param file_path 音乐文件路径
         * @return 加载成功返回 MIX_Audio 指针，失败返回 nullptr
         */
        MIX_Audio *loadMusic(std::string_view file_path);

        /**
         * @brief 获取音乐资源（缓存优先）
         * @param file_path 音乐文件路径
         * @return 成功返回 MIX_Audio 指针，失败抛出异常
         */
        MIX_Audio *getMusic(std::string_view file_path);

        /**
         * @brief 卸载指定音乐资源
         * @param file_path 音乐文件路径
         */
        void unloadMusic(std::string_view file_path);

        /**
         * @brief 清空所有音乐资源
         */
        void clearMusic();

        /**
         * @brief 清空所有音频资源
         */
        void clearAudio();
    };

} // namespace engine::resource