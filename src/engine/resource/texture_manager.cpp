// ==============================
// 标准库头文件
// ==============================

// ==============================
// 其它文件引入
// ==============================
#include "texture_manager.h"

// ==============================
// 第三方库头文件
// ==============================
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

// ==============================
// 命名空间与实现
// ==============================
namespace engine::resource
{
    /**
     * @brief 纹理管理器的构造函数，初始化纹理管理器
     *
     * @param renderer SDL渲染器指针（必须非空）
     * @throw std::runtime_error 渲染器指针为空时抛出异常
     */
    TextureManager::TextureManager(SDL_Renderer *renderer)
        : renderer_(renderer)
    {
        if (!renderer_)
        {
            throw std::runtime_error("纹理管理器构造失败：渲染器指针为空");
        }
        spdlog::info("纹理管理器构造成功");
    }

    /**
     * @brief 加载纹理资源（不存在则加载，已存在则返回缓存）
     *
     * @param file_path 纹理文件路径（支持BMP/PNG/JPG等）
     * @return 成功返回SDL_Texture指针，失败返回nullptr
     */
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

    /**
     * @brief 获取已加载的纹理（不存在则尝试自动加载）
     *
     * @param file_path 纹理文件路径
     * @return 成功返回SDL_Texture指针，失败返回nullptr
     */
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

    /**
     * @brief 卸载指定路径的纹理资源
     *
     * @param file_path 纹理文件路径
     */
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

    /**
     * @brief 获取纹理资源的尺寸（宽/高）
     *
     * @param file_path 纹理文件路径
     * @return 包含宽高的glm::vec2（x=宽，y=高），失败返回(0,0)
     */
    glm::vec2 TextureManager::getTextureSize(const std::string &file_path)
    {
        SDL_Texture *texture = getTexture(file_path);
        if (!texture)
        {
            spdlog::error("无法获取纹理：{}", file_path);
            return glm::vec2(0);
        }

        glm::vec2 size; // 完全保留原代码的size变量定义
        if (!SDL_GetTextureSize(texture, &size.x, &size.y))
        {
            spdlog::error("无法查询纹理尺寸：{}", file_path);
            return glm::vec2(0);
        }
        return size;
    }

    /**
     * @brief 清空所有已加载的纹理资源
     */
    void TextureManager::clearTextures()
    {
        textures_.clear();
        spdlog::info("所有纹理已清空");
    }

} // namespace engine::resource
