#include "camera.h"
#include "../utils/math.h"
#include <spdlog/spdlog.h>

namespace engine::render
{
    Camera::Camera(glm::vec2 viewport_size, glm::vec2 position, std::optional<engine::utils::Rect> limit_bounds) : viewport_size_(viewport_size), position_(position), limit_bounds_(limit_bounds)
    {
        spdlog::trace("相机初始化成功，位置{},{}", position_.x, position_.y);
    }

    void Camera::update(float delta_time)
    {
        // TODO
    }

    void Camera::move(const glm::vec2 &offset)
    {
        position_ += offset;
        clampPosition();
    }

    // translation
    glm::vec2 Camera::worldToScreen(const glm::vec2 &world_pos) const
    {
        return world_pos - position_;
    }

    glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_factor) const
    {
        return world_pos - position_ * scroll_factor;
    }

    glm::vec2 Camera::screenToWorld(const glm::vec2 &screen_pos) const
    {
        return screen_pos + position_;
    }

    // getter setter
    void Camera::setPosition(glm::vec2 position)
    {
        position_ = position;
    }

    void Camera::setLimitBounds(std::optional<engine::utils::Rect> limit_bounds)
    {
        limit_bounds_ = limit_bounds;
        clampPosition();
    }

    const glm::vec2 &Camera::getPosition() const
    {
        return position_;
    }

    std::optional<engine::utils::Rect> Camera::getLimitBounds() const
    {
        return limit_bounds_;
    }

    glm::vec2 Camera::getViewportSize() const
    {
        return viewport_size_;
    }

    void Camera::clampPosition()
    {
        // 边界检查需要确保相机视图在limit_bounds内
        if (limit_bounds_.has_value() && limit_bounds_->size.x > 0 && limit_bounds_->size.y > 0)
        {
            // 计算允许的相机位置
            glm::vec2 min_cam_pos = limit_bounds_->position;
            glm::vec2 max_cam_pos = limit_bounds_->position + limit_bounds_->size - viewport_size_;

            // 确保max_cam_pos不小于min_cam_pos（视口可能比世界大）相机不动
            max_cam_pos.x = std::max(min_cam_pos.x, max_cam_pos.x);
            max_cam_pos.y = std::max(min_cam_pos.y, max_cam_pos.y);

            position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
        }
        // 若limit_bounds无效则不限制
    }
}