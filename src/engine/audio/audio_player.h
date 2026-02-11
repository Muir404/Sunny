#pragma once
#include <string>
#include <string_view>
#include <optional>

struct MIX_Audio;
struct MIX_Track;
struct MIX_Mixer;

namespace engine::resource
{
    class ResourceManager;
}

namespace engine::audio
{
    class AudioPlayer final
    {
    private:
        engine::resource::ResourceManager *resource_manager_;
        std::string current_music_path_;
        MIX_Track *music_track_ = nullptr;
        float music_volume_ = 1.0f;
        float sound_volume_ = 1.0f;

        // 辅助函数只内部用，不对外暴露
        MIX_Track *getMusicTrack();
        std::optional<MIX_Track *> getSoundTrack();
        void releaseSoundTrack(MIX_Track *track);

    public:
        explicit AudioPlayer(engine::resource::ResourceManager *resource_manager);
        ~AudioPlayer();

        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;
        AudioPlayer(AudioPlayer &&) = delete;
        AudioPlayer &operator=(AudioPlayer &&) = delete;

        // ================= 你的原版接口完全不动 =================
        MIX_Track *playSound(std::string_view sound_path, std::optional<float> volume = std::nullopt);
        bool playMusic(std::string_view music_path, int loops = -1, int fade_in_ms = 0);
        void stopMusic(int fade_out_ms = 0);
        void pauseMusic();
        void resumeMusic();

        void setSoundVolume(float volume);
        void setMusicVolume(float volume);
        float getMusicVolume() const;
        float getSoundVolume() const;
        bool setTrackVolume(MIX_Track *track, float volume);
    };

} // namespace engine::audio
