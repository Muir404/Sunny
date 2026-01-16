#pragma once
#include <memory>            // 用于 std::unique_ptr
#include <stdexcept>         // 用于 std::runtime_error
#include <string>            // 用于 std::string
#include <string_view>       // 用于 std::string_view
#include <unordered_map>     // 用于 std::unordered_map
#include <SDL3/SDL_render.h> // 用于 SDL_Texture 和 SDL_Renderer
#include <glm/glm.hpp>

namespace engine::resource
{
    class TextureManager
    {
        friend class ResourceManager;

    private:
        // 构造删除器管理指针
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

        // 存储文件路径和指向管理纹理的 unique_ptr 的映射。(容器的键不可使用std::string_view)
        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;

        SDL_Renderer *renderer_ = nullptr; // 指向主渲染器的非拥有指针

    public:
        explicit TextureManager(SDL_Renderer *renderer);

        // 删除无用部分
        TextureManager(const TextureManager &) = delete;
        TextureManager &operator=(const TextureManager &) = delete;
        TextureManager(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager &&) = delete;

    private:
        SDL_Texture *loadTexture(const std::string &file_path); // 载入纹理
        SDL_Texture *getTexture(const std::string &file_path);  // 获取已经加载纹理资源的指针，无的尝试加载
        void unloadTexture(const std::string &file_path);       // 卸载纹理资源
        glm::vec2 getTextureSize(const std::string &file_path); // 获取纹理资源的尺寸
        void clearTextures();                                   // 清理所有纹理资源
    };
};