#include "texture_manager.h"
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{
    TextureManager::TextureManager(SDL_Renderer *renderer) : renderer_(renderer)
    {
        if (!renderer_)
        {
            throw std::runtime_error("纹理管理器构造失败：渲染器指针为空");
        }
        spdlog::trace("纹理管理器构造成功");
    }

    SDL_Texture *TextureManager::loadTexture(const std::string &file_path)
    {
        // 检查是否已经加载
        auto it = textures_.find(file_path);
        if (it != textures_.end())
        {
            return it->second.get();
        }

        // 如果没有加载则尝试加载
        SDL_Texture *raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());

        if (!SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST)) // 最邻近插值优化画面
        {
            spdlog::warn("无法设置纹理模式为最邻近插值");
        }

        if (!raw_texture)
        {
            spdlog::error("加载纹理失败：'{}':{}", file_path, SDL_GetError());
            return nullptr;
        }
        textures_.emplace(file_path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
        spdlog::debug("成功载入并缓存纹理：{}", file_path);

        return raw_texture;
    }

    SDL_Texture *TextureManager::getTexture(const std::string &file_path)
    {
        auto it = textures_.find(file_path);
        if (it != textures_.end())
        {
            return it->second.get();
        }

        spdlog::warn("未缓存纹理'{}'，尝试加载它", file_path);

        return loadTexture(file_path);
    }

    void TextureManager::unloadTexture(const std::string &file_path)
    {
        auto it = textures_.find(file_path);
        if (it != textures_.end())
        {
            spdlog::debug("卸载纹理：{}", file_path);
            textures_.erase(it); // 借助删除器删除
        }
        else
        {
            spdlog::warn("尝试删除不存在的纹理：{}", file_path);
        }
    }

    glm::vec2 TextureManager::getTextureSize(const std::string &file_path)
    {
        SDL_Texture *texture = getTexture(file_path);
        if (!texture)
        {
            spdlog::error("无法获取纹理：{}", file_path);
            return glm::vec2(0);
        }

        glm::vec2 size;
        if (!SDL_GetTextureSize(texture, &size.x, &size.y))
        {
            spdlog::error("无法查询纹理尺寸：{}", file_path);
            return glm::vec2(0);
        }
        return glm::vec2();
    }

    void TextureManager::clearTextures()
    {
        textures_.clear();
    }

} // namespace engine::resource
