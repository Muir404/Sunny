#pragma once

// ==============================
// 标准库头文件
// ==============================
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

// ==============================
// 其它文件引入
// ==============================

// ==============================
// 第三方库头文件
// ==============================
#include <SDL3/SDL_render.h>
#include <glm/glm.hpp>

// ==============================
// 命名空间与实现
// ==============================
namespace engine::resource
{
    /**
     * @class TextureManager
     * @brief 纹理资源管理器类
     *
     * 负责SDL纹理的加载、缓存、卸载和尺寸查询，使用智能指针+自定义删除器管理纹理生命周期，
     * 避免内存泄漏。仅允许ResourceManager作为友元类访问，确保资源管理的唯一性。
     */
    class TextureManager
    {
        // 仅允许ResourceManager访问私有接口，封装资源管理逻辑
        friend class ResourceManager;

    private:
        /**
         * @struct SDLTextureDeleter
         * @brief SDL_Texture智能指针删除器
         *
         * 确保销毁时调用SDL_DestroyTexture释放纹理资源
         */
        struct SDLTextureDeleter
        {
            void operator()(SDL_Texture *texture) const
            {
                if (texture)
                {
                    SDL_DestroyTexture(texture);
                }
            }
        };

        // 私有成员变量
        // 纹理缓存映射表：文件路径 → 带自定义删除器的SDL_Texture智能指针
        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;
        // SDL渲染器指针（非拥有权，由外部ResourceManager传入并保证生命周期）
        SDL_Renderer *renderer_ = nullptr;

    public:
        /**
         * @brief 构造函数：初始化纹理管理器
         *
         * @param renderer SDL渲染器指针（必须非空）
         * @throw std::runtime_error 渲染器指针为空时抛出异常
         */
        explicit TextureManager(SDL_Renderer *renderer);

        // 禁用拷贝/移动语义
        // 确保纹理管理器实例唯一，避免资源重复释放
        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        /**
         * @brief 加载纹理资源（不存在则加载，已存在则返回缓存）
         *
         * @param file_path 纹理文件路径（支持BMP/PNG/JPG等）
         * @return 成功返回SDL_Texture指针，失败返回nullptr
         */
        SDL_Texture *loadTexture(std::string_view file_path);

        /**
         * @brief 获取已加载的纹理（不存在则尝试自动加载）
         *
         * @param file_path 纹理文件路径
         * @return 成功返回SDL_Texture指针，失败返回nullptr
         */
        SDL_Texture *getTexture(std::string_view file_path);

        /**
         * @brief 卸载指定路径的纹理资源
         *
         * @param file_path 纹理文件路径
         */
        void unloadTexture(std::string_view file_path);

        /**
         * @brief 获取纹理资源的尺寸（宽/高）
         *
         * @param file_path 纹理文件路径
         * @return 包含宽高的glm::vec2（x=宽，y=高），失败返回(0,0)
         */
        glm::vec2 getTextureSize(std::string_view file_path);

        /**
         * @brief 清空所有已加载的纹理资源
         */
        void clearTextures();
    };
}
