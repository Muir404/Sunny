#pragma once
#include <memory>        // 用于 std::unique_ptr
#include <stdexcept>     // 用于 std::runtime_error
#include <string>        // 用于 std::string
#include <string_view>   // 用于 std::string_view
#include <unordered_map> // 用于 std::unordered_map
#include <utility>       // 用于 std::pair
#include <functional>    // 用于 std::hash

#include <SDL3_ttf/SDL_ttf.h> // SDL_ttf 主头文件

namespace engine::resource
{
    using FontKey = std::pair<std::string, int>;

    struct FontKeyHash
    {
        std::size_t operator()(const FontKey &key) const
        {
            std::hash<std::string> string_hasher;
            std::hash<int> int_hasher;
            return string_hasher(key.first) ^ int_hasher(key.second);
        }
    };

    class FontManager
    {
        friend class ResourceManager;

    private:
        // 构造删除器管理指针
        struct SDLFontDeleter
        {
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };

        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> fonts_;

    public:
        FontManager();
        ~FontManager();

        // 删除无用部分
        FontManager(const FontManager &) = delete;
        FontManager &operator=(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager &operator=(FontManager &&) = delete;

    private:
        // Fonts
        TTF_Font *loadFont(std::string_view file_path, int point_size); // 载入字体资源
        TTF_Font *getFont(std::string_view file_path, int point_size);  // 获取已经加载字体资源的指针，无的尝试加载
        void unloadFont(std::string_view file_path, int point_size);    // 卸载字体资源
        void clearFonts();                                              // 清理所有字体资源
    };
};