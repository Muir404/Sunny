#pragma once
#include <memory>        // 用于 std::unique_ptr
#include <stdexcept>     // 用于 std::runtime_error
#include <string>        // 用于 std::string
#include <string_view>   // 用于 std::string_view
#include <unordered_map> // 用于 std::unordered_map

#include <SDL3_mixer/SDL_mixer.h> // SDL_mixer 主头文件

namespace engine::resource
{
    class AudioManager
    {
        friend class ResourceManager;

    private:
        // 构造删除器管理指针
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

        // 处理音效
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> sounds_;
        // 处理音乐
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> music_;

        MIX_Mixer *m_mixer = nullptr;

    public:
        AudioManager();
        ~AudioManager(); //

        // 删除无用部分
        AudioManager(const AudioManager &) = delete;
        AudioManager &operator=(const AudioManager &) = delete;
        AudioManager(AudioManager &&) = delete;
        AudioManager &operator=(AudioManager &&) = delete;

    private:
        void clearAudio();

        // Sound Effects(Chunks)
        MIX_Audio *loadSound(const std::string &file_path); // 载入音效资源
        MIX_Audio *getSound(const std::string &file_path);  // 获取已经加载音效资源的指针，无的尝试加载
        void unloadSound(const std::string &file_path);     // 卸载音效资源
        void clearSounds();                                 // 清理所有音效资源

        // Music
        MIX_Audio *loadMusic(const std::string &file_path); // 载入音乐资源
        MIX_Audio *getMusic(const std::string &file_path);  // 获取已经加载音乐资源的指针，无的尝试加载
        void unloadMusic(const std::string &file_path);     // 卸载音乐资源
        void clearMusic();                                  // 清理所有音乐资源
    };
};