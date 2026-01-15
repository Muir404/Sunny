#pragma once

#include <SDL3/SDL.h>

namespace engine::core // 命名空间与路径一致
{
    class Time
    {
    private:
        Uint64 last_time_ = 0;        // 上一帧的时间戳
        Uint64 frame_start_time_ = 0; // 当前帧开始的时间戳
        double delta_time_ = 0.0f;    // 未缩放的帧时间差
        double time_scale_ = 1.0f;    // 时间缩放因子

        int target_fps_ = 0;              // 目标FPS
        double target_frame_time_ = 0.0f; // 目标每帧时间

    public:
        Time();

        void update();

        float getDeltaTime() const;
        float getUnscaledDeltaTime() const;

        void setTimeScale(float scale);
        float getTimeScale() const;

        void setTargetFps(int fps);
        int getTargetFps() const;

    private:
        void limitFrameRate(float current_delta_time);
    };
}