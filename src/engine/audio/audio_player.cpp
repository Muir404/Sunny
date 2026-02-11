#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <stdexcept>

namespace engine::audio
{
    // ==============================================
    // 【重要】全程只和 resource_manager_ 交互
    // 不直接访问 AudioManager！不直接调用 AudioManager！
    // ==============================================

    AudioPlayer::AudioPlayer(engine::resource::ResourceManager *resource_manager)
        : resource_manager_(resource_manager)
    {
        if (!resource_manager_)
        {
            throw std::runtime_error("AudioPlayer: ResourceManager is null");
        }

        // 获取音乐轨道（通过 ResourceManager 门面）
        music_track_ = resource_manager_->getMusicTrack();
        if (!music_track_)
        {
            throw std::runtime_error("AudioPlayer: Failed to get music track");
        }
    }

    AudioPlayer::~AudioPlayer()
    {
        if (music_track_)
        {
            stopMusic(200);
            MIX_SetTrackAudio(music_track_, nullptr);
        }
    }

    // 获取音乐轨道（门面调用）
    MIX_Track *AudioPlayer::getMusicTrack()
    {
        return resource_manager_->getMusicTrack();
    }

    // 获取音效临时轨道（门面调用）
    std::optional<MIX_Track *> AudioPlayer::getSoundTrack()
    {
        MIX_Track *track = resource_manager_->getSoundTrack();
        if (!track)
            return std::nullopt;

        MIX_SetTrackGain(track, sound_volume_);
        // resource_manager_->tagTrack(track, "sound");
        return track;
    }

    // 释放临时轨道（门面调用）
    void AudioPlayer::releaseSoundTrack(MIX_Track *track)
    {
        resource_manager_->releaseSoundTrack(track);
    }

    // ====================== 播放音效 ======================
    MIX_Track *AudioPlayer::playSound(std::string_view sound_path, std::optional<float> volume)
    {
        // 只走 ResourceManager
        MIX_Audio *sound = resource_manager_->getSound(std::string(sound_path));
        if (!sound)
        {
            spdlog::error("AudioPlayer: missing sound {}", sound_path);
            return nullptr;
        }

        auto track_opt = getSoundTrack();
        if (!track_opt)
            return nullptr;
        MIX_Track *track = *track_opt;

        MIX_SetTrackAudio(track, sound);

        if (volume)
        {
            setTrackVolume(track, *volume);
        }

        SDL_PropertiesID props = SDL_CreateProperties();
        // SDL_SetProperty(props, MIX_PROP_TRACK_LOOPS_INT, 0);
        MIX_PlayTrack(track, props);
        SDL_DestroyProperties(props);

        // 播放完自动归还
        // MIX_SetTrackStoppedCallback(track, [](MIX_Track *t, void *ud)
        //                             {
        //     auto self = static_cast<AudioPlayer*>(ud);
        //     self->releaseSoundTrack(t); }, this);

        return track;
    }

    // ====================== 播放音乐 ======================
    bool AudioPlayer::playMusic(std::string_view music_path, int loops, int fade_in_ms)
    {
        if (music_path == current_music_path_)
            return true;

        // 只走 ResourceManager
        MIX_Audio *music = resource_manager_->getMusic(std::string(music_path));
        if (!music)
            return false;

        stopMusic(fade_in_ms / 2);
        current_music_path_ = music_path;

        MIX_SetTrackAudio(music_track_, music);
        MIX_SetTrackGain(music_track_, music_volume_);

        SDL_PropertiesID props = SDL_CreateProperties();
        // SDL_SetProperty(props, MIX_PROP_TRACK_LOOPS_INT, loops);

        if (fade_in_ms > 0)
        {
            Sint64 fade = MIX_TrackMSToFrames(music_track_, fade_in_ms);
            // SDL_SetProperty(props, MIX_PROP_TRACK_FADE_IN_FRAMES_INT64, fade);
        }

        bool ok = MIX_PlayTrack(music_track_, props);
        SDL_DestroyProperties(props);

        return ok;
    }

    // ====================== 音乐控制 ======================
    void AudioPlayer::stopMusic(int fade_out_ms)
    {
        if (!music_track_)
            return;
        Sint64 fade = fade_out_ms > 0 ? MIX_TrackMSToFrames(music_track_, fade_out_ms) : 0;
        MIX_StopTrack(music_track_, fade);
        current_music_path_.clear();
    }

    void AudioPlayer::pauseMusic()
    {
        if (MIX_TrackPlaying(music_track_))
            MIX_PauseTrack(music_track_);
    }

    void AudioPlayer::resumeMusic()
    {
        if (MIX_TrackPaused(music_track_))
            MIX_ResumeTrack(music_track_);
    }

    // ====================== 音量 ======================
    void AudioPlayer::setSoundVolume(float volume)
    {
        sound_volume_ = glm::clamp(volume, 0.0f, 1.0f);
        resource_manager_->setSoundTagGain(sound_volume_);
    }

    void AudioPlayer::setMusicVolume(float volume)
    {
        music_volume_ = glm::clamp(volume, 0.0f, 1.0f);
        MIX_SetTrackGain(music_track_, music_volume_);
    }

    float AudioPlayer::getMusicVolume() const
    {
        return MIX_GetTrackGain(music_track_);
    }

    float AudioPlayer::getSoundVolume() const
    {
        return sound_volume_;
    }

    bool AudioPlayer::setTrackVolume(MIX_Track *track, float volume)
    {
        return MIX_SetTrackGain(track, glm::clamp(volume, 0.0f, 1.0f));
    }

} // namespace engine::audio
