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

        // ========================== 类型定义（Type Definitions）==========================
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

        /**
         * @brief MIX_Track (轨道) 自定义删除器
         * 确保音频轨道资源被正确释放
         */
        struct TrackDeleter
        {
            void operator()(MIX_Track *track) const
            {
                if (track)
                {
                    MIX_DestroyTrack(track);
                }
            }
        };

        /**
         * @brief MIX_Group (分组) 自定义删除器
         * 确保音频分组资源被正确释放
         */
        struct GroupDeleter
        {
            void operator()(MIX_Group *group) const
            {
                if (group)
                {
                    MIX_DestroyGroup(group);
                }
            }
        };

        // ========================== 常量定义（Constants）==========================
    private:
        /// 轨道池默认容量（预分配轨道数量）
        static constexpr size_t DEFAULT_TRACK_POOL_SIZE = 16;

        // ========================== 成员变量（Member Variables）==========================
    private:
        // 核心混音器对象
        MIX_Mixer *mixer_ = nullptr;

        // 资源缓存（SDL_mixer 3.0 中音效/音乐统一为 MIX_Audio，按用途区分缓存）
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> sounds_; // 短音效缓存 (路径 -> 音频资源)
        std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>> music_;  // 长音乐缓存 (路径 -> 音频资源)

        // 轨道管理
        std::unordered_map<std::string, std::unique_ptr<MIX_Track, TrackDeleter>> named_tracks_; // 命名轨道 (名称 -> 轨道对象)
        std::vector<std::unique_ptr<MIX_Track, TrackDeleter>> track_pool_;                       // 临时轨道对象池

        // 分组管理
        std::unordered_map<std::string, std::unique_ptr<MIX_Group, GroupDeleter>> audio_groups_; // 音频分组 (名称 -> 分组对象)

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
        // -------------------------- 资源加载/卸载 --------------------------
        /**
         * @brief 加载音效资源（短音频，预解码到内存）
         * @param file_path 音效文件路径
         * @return 加载成功返回 MIX_Audio 指针，失败返回 nullptr
         */
        MIX_Audio *loadSound(const std::string &file_path);

        /**
         * @brief 获取音效资源（缓存优先）
         * @param file_path 音效文件路径
         * @return 成功返回 MIX_Audio 指针，失败抛出异常
         */
        MIX_Audio *getSound(const std::string &file_path);

        /**
         * @brief 卸载指定音效资源
         * @param file_path 音效文件路径
         */
        void unloadSound(const std::string &file_path);

        /**
         * @brief 清空所有音效资源
         */
        void clearSounds();

        /**
         * @brief 加载音乐资源（长音频，流式解码）
         * @param file_path 音乐文件路径
         * @return 加载成功返回 MIX_Audio 指针，失败返回 nullptr
         */
        MIX_Audio *loadMusic(const std::string &file_path);

        /**
         * @brief 获取音乐资源（缓存优先）
         * @param file_path 音乐文件路径
         * @return 成功返回 MIX_Audio 指针，失败抛出异常
         */
        MIX_Audio *getMusic(const std::string &file_path);

        /**
         * @brief 卸载指定音乐资源
         * @param file_path 音乐文件路径
         */
        void unloadMusic(const std::string &file_path);

        /**
         * @brief 清空所有音乐资源
         */
        void clearMusic();

        // -------------------------- 轨道管理 --------------------------
        /**
         * @brief 获取命名轨道（不存在则创建）
         * @param track_name 轨道名称
         * @return 成功返回轨道指针，失败返回 nullptr
         */
        MIX_Track *getNamedTrack(const std::string &track_name);

        /**
         * @brief 获取临时轨道（从对象池分配）
         * @return 成功返回轨道指针，失败返回 nullptr
         */
        MIX_Track *getTemporaryTrack();

        /**
         * @brief 释放临时轨道（归还到对象池）
         * @param track 要释放的轨道指针
         */
        void releaseTemporaryTrack(MIX_Track *track);

        /**
         * @brief 创建轨道对象池
         * @param poolSize 池容量
         */
        void createTrackPool(size_t poolSize);

        // -------------------------- 分组管理 --------------------------
        /**
         * @brief 创建音频分组（不存在则创建）
         * @param group_name 分组名称
         * @return 成功返回分组指针，失败返回 nullptr
         */
        MIX_Group *createGroup(const std::string &group_name);

        /**
         * @brief 将轨道添加到分组
         * @param track 轨道指针
         * @param group 分组指针
         * @return 添加成功返回 true，失败返回 false
         */
        bool addTrackToGroup(MIX_Track *track, MIX_Group *group);

        // -------------------------- 轨道标签 --------------------------
        /**
         * @brief 为轨道添加标签
         * @param track 轨道指针
         * @param tag 标签名称
         * @return 成功返回 true，失败返回 false
         */
        bool tagTrack(MIX_Track *track, const std::string &tag);

        /**
         * @brief 移除轨道的指定标签
         * @param track 轨道指针
         * @param tag 标签名称
         * @return 成功返回 true，失败返回 false
         */
        bool untagTrack(MIX_Track *track, const std::string &tag);

        // -------------------------- 混音器控制 --------------------------
        /**
         * @brief 设置主音量
         * @param gain 音量值（0.0 ~ 1.0）
         * @return 设置成功返回 true，失败返回 false
         */
        bool setMasterGain(float gain);

        /**
         * @brief 获取核心混音器对象
         * @return 混音器指针
         */
        MIX_Mixer *getMixer() const
        {
            return mixer_;
        }

        // -------------------------- 资源清理 --------------------------
        /**
         * @brief 清空混音器资源
         */
        void clearMixer();

        /**
         * @brief 清空所有音频资源
         * 包括音效、音乐、轨道和分组
         */
        void clearAudio();
        void clearNamedTracks();

        MIX_Track *loadNamedTrack(const std::string &track_name);
        void AudioManagerunloadNamedTrack(const std::string &track_name);
        void unloadNamedTrack(const std::string &track_name);
        void clearTrackPool();
        MIX_Group *loadGroup(const std::string &group_name);
        MIX_Group *getGroup(const std::string &group_name);
        void unloadGroup(const std::string &group_name);

        void clearGroups();
        bool setTrackGain(MIX_Track *track, float gain);
    };

} // namespace engine::resource