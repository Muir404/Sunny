#pragma once
#include <memory> // 用于 std::unique_ptr
#include <string> // 用于 std::string
#include <glm/glm.hpp>

// 前向声明 SDL 类型
struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct TTF_Font;

namespace engine::resource
{
    class TextureManager;
    class AudioManager;
    class FontManager;

    class ResourceManager
    {
    private:
        std::unique_ptr<TextureManager> texture_manager_;
        std::unique_ptr<AudioManager> audio_manager_;
        std::unique_ptr<FontManager> font_manager_;

    public:
        explicit ResourceManager(SDL_Renderer *renderer);

        ~ResourceManager(); // 显式声明析构函数，能让智能指针正确管理有的向前声明的类

        void clear();

        // 删除无用部分
        ResourceManager(const ResourceManager &) = delete;
        ResourceManager &operator=(const ResourceManager &) = delete;
        ResourceManager(ResourceManager &&) = delete;
        ResourceManager &operator=(ResourceManager &&) = delete;

        // --- 统一的资源访问接口 ---
        // Texture
        SDL_Texture *loadTexture(const std::string &file_path); // 载入纹理
        SDL_Texture *getTexture(const std::string &file_path);  // 获取已经加载纹理资源的指针，无的尝试加载
        void unloadTexture(const std::string &file_path);       // 卸载纹理资源
        glm::vec2 getTextureSize(const std::string &file_path); // 获取纹理资源的尺寸
        void clearTextures();                                   // 清理所有纹理资源

        // Sound Effects(Chunks)
        MIX_Audio *loadSound(const std::string &file_path); // 载入音效资源
        MIX_Audio *getSound(const std::string &file_path);  // 获取已经加载音效资源的指针，无的尝试加载
        void unloadSound(const std::string &file_path);     // 卸载音效资源
        void clearSounds();                                 // 清理所有音效资源

        // Music
        MIX_Audio *loadMusic(const std::string &file_path); // 载入音乐资源
        MIX_Audio *getMusic(const std::string &file_path);  // 获取已经加载音乐资源的指针，无的尝试加载
        void unloadMusic(const std::string &file_path);     // 卸载音乐资源
        void clearMusic();                                  // 清理所有音乐资源

        // Fonts
        TTF_Font *loadFont(std::string file_path, int point_size); // 载入字体资源
        TTF_Font *getFont(std::string file_path, int point_size);  // 获取已经加载字体资源的指针，无的尝试加载
        void unloadFont(std::string file_path, int point_size);    // 卸载字体资源
        void clearFonts();                                         // 清理所有字体资源
    };
};