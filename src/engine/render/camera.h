#pragma once
#include "../utils/math.h"
#include <optional>

namespace engine::render
{
    class Camera final
    {
    private:
        glm::vec2 viewport_size_;                         // 视口大小（屏幕大小）
        glm::vec2 position_;                              // 相机左上角世界坐标
        std::optional<engine::utils::Rect> limit_bounds_; // 限制相机移动范围，无值表示不限制

    public:
        Camera(glm::vec2 viewport_size, glm::vec2 position = glm::vec2(0.0f, 0.0f), std::optional<engine::utils::Rect> limit_bounds = std::nullopt);

        void update(float delta_time);      // 更新相机位置
        void move(const glm::vec2 &offset); // 移动相机

        glm::vec2 worldToScreen(const glm::vec2 &world_pos) const;                                             // 世界坐标转屏幕坐标
        glm::vec2 worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_factor) const; // 世界坐标转屏幕坐标，考虑视差滚动
        glm::vec2 screenToWorld(const glm::vec2 &screen_pos) const;                                            /// 屏幕坐标转世界坐标

        void setPosition(glm::vec2 position);                                 // 设置相机位置
        void setLimitBounds(std::optional<engine::utils::Rect> limit_bounds); // 设置限制相机的移动范围

        const glm::vec2 &getPosition() const;                      // 获取相机位置
        std::optional<engine::utils::Rect> getLimitBounds() const; // 获取限制相机的移动范围
        glm::vec2 getViewportSize() const;                         // 获取视口大小

        // 禁用拷贝和移动语义
        Camera(const Camera &) = delete;
        Camera &operator=(const Camera &) = delete;
        Camera(Camera &&) = delete;
        Camera &operator=(Camera &&) = delete;

    private:
        void clampPosition(); //限制相机位置在边界内
    };
}