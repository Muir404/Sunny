#pragma once
#include <SDL3/SDL_rect.h> // 用于 SDL——FRect
#include <optional>        // 用于 std::optional 表示可选的源矩形
#include <string>

namespace engine::render
{
    // 包含纹理标识符和要绘制的纹理部分以及翻转状态
    class Sprite final
    {
    private:
        std::string texture_id_;               // 纹理资源标识符
        std::optional<SDL_FRect> source_rect_; // 可选：要绘制的纹理部分
        bool is_flipped_ = false;              // 是否水平翻转

    public:
        Sprite() = default;

        Sprite(const std::string &texture_id, const std::optional<SDL_FRect> source_rect = std::nullopt, bool is_flipped = false)
            : texture_id_(texture_id),
              source_rect_(source_rect),
              is_flipped_((is_flipped))
        {
        }

        // getters and setters
        const std::string &getTextureId() const { return texture_id_; }               // 获取纹理ID
        const std::optional<SDL_FRect> getSourceRect() const { return source_rect_; } // 获取源矩形
        bool isFlipped() const { return is_flipped_; }                                // 获取是否水平翻转

        void setTextureId(const std::string &texture_id) { texture_id_ = texture_id; }                 // 设置纹理ID
        void setSourceRect(const std::optional<SDL_FRect> source_rect) { source_rect_ = source_rect; } // 设置源矩形
        void setFlipped(bool flipped) { is_flipped_ = flipped; }                                       // 设置是否水平翻转
    };
}