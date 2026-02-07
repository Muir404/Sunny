#include "camera.h"
#include "../utils/math.h"
#include "../component/transform_component.h"
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
        if (target_ == nullptr)
        {
            return;
        }
        glm::vec2 target_pos = target_->getPosition();
        glm::vec2 desired_position = target_pos - viewport_size_ / 2.0f; // 计算目标位置 (让目标位于视口中心)

        // 计算当前位置与目标位置的距离
        auto distance_ = glm::distance(position_, desired_position);
        constexpr float SNAP_THRESHOLD = 1.0f; // 设置一个距离阈值  (constexpr: 编译时常量，避免每次调用都计算)

        if (distance_ < SNAP_THRESHOLD)
        {
            // 如果距离小于阈值，直接吸附到目标位置
            position_ = desired_position;
        }
        else
        {
            // 否则，使用线性插值平滑移动   glm::mix(a,b,t): 在向量 a 和 b 之间进行插值，t 是插值因子，范围在0到1之间。
            // 公式: (b-a)*t + a;   t = 0 时结果为 a，t = 1 时结果为 b
            position_ = glm::mix(position_, desired_position, smooth_speed_ * delta_time);
            position_ = glm::vec2(glm::round(position_.x), glm::round(position_.y)); // 四舍五入到整数,省略的话偶尔会出现画面割裂
        }

        clampPosition();
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

    void Camera::setTarget(engine::component::TransformComponent *target)
    {
        target_ = target;
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

    engine::component::TransformComponent *Camera::getTarget() const
    {
        return target_;
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