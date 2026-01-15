#include "time.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace engine::core // 命名空间与路径一致
{
    Time::Time()
    {
        // 初始化last_time_和frame_start_time_为当前时间，避免第一帧的DeltaTime过大
        last_time_ = SDL_GetTicksNS();
        frame_start_time_ = last_time_;
        spdlog::trace("Time初始化。Last time：{}", last_time_);
    }

    void Time::update()
    {
        frame_start_time_ = SDL_GetTicksNS();                                                // 记录进入update时的时间戳
        auto current_delta_time = static_cast<double>(frame_start_time_ - last_time_) / 1e9; // 当前帧时间差
        if (target_frame_time_ > 0.0)                                                        // 如果设置了目标帧率，则限制帧率;否则delta_time_ = current_delta_time
        {
            limitFrameRate(current_delta_time);
        }
        else
        {
            delta_time_ = current_delta_time;
        }

        last_time_ = SDL_GetTicksNS(); // 记录离开update时的时间戳
    }

    float Time::getDeltaTime() const
    {
        return delta_time_ * time_scale_;
    }

    float Time::getUnscaledDeltaTime() const
    {
        return delta_time_;
    }

    void Time::setTimeScale(float scale)
    {
        if (scale < 0.0)
        {
            spdlog::warn("Time scale不能为负数。限制为0");
            scale = 0.0;
        }
        time_scale_ = scale;
    }

    float Time::getTimeScale() const
    {
        return time_scale_;
    }

    void Time::setTargetFps(int fps)
    {
        if (fps < 0)
        {
            spdlog::warn("目标帧率不能为负数。限制为0（不限制帧率）");
            fps = 0;
        }
        else
        {
            target_fps_ = fps;
        }

        if (target_fps_ > 0)
        {
            target_frame_time_ = 1.0 / static_cast<double>(target_fps_);
            spdlog::info("目标帧率设置为: {} (Frame time: {:.6f}s)", target_fps_, target_frame_time_);
        }
        else
        {
            target_frame_time_ = 0.0;
            spdlog::info("目标帧率设置为: Unlimited");
        }
    }

    int Time::getTargetFps() const
    {
        return target_fps_;
    }

    void Time::limitFrameRate(float current_delta_time)
    {
        // 如果当前耗费的时间小于目标帧时间，则等待剩余时间
        if (current_delta_time < target_frame_time_)
        {
            double time_to_wait = target_frame_time_ - current_delta_time;
            Uint64 ns_to_wait = static_cast<Uint64>(time_to_wait * 1e9);
            SDL_DelayNS(ns_to_wait);
            delta_time_ = static_cast<double>(SDL_GetTicksNS() - last_time_) / 1e9;
        }
        else
        {
            // 否则，直接使用当前帧耗费的时间
            // 限制最大 delta time 为 0.1秒 (即最低 10 FPS)，防止刚启动或卡顿时的物理穿模
            delta_time_ = std::min(static_cast<double>(current_delta_time), 0.1);
        }
    }
}