#pragma once

// ==============================
// 标准库头文件
// ==============================
#include <functional>    // 用于 std::hash
#include <memory>        // 用于 std::unique_ptr
#include <stdexcept>     // 用于 std::runtime_error
#include <string>        // 用于 std::string
#include <string_view>   // 用于 const std::string&
#include <unordered_map> // 用于 std::unordered_map
#include <utility>       // 用于 std::pair

// ==============================
// 其它文件引入
// ==============================

// ==============================
// 第三方库头文件
// ==============================
#include <SDL3_ttf/SDL_ttf.h> // SDL_ttf 主头文件

// ==============================
// 命名空间与实现
// ==============================
namespace engine::resource
{
    /**
     * @brief 字体资源的索引键值类型
     *
     * 由字体文件路径(std::string)和字体大小(int)组成的配对，
     * 用于区分不同路径/不同大小的字体资源
     */
    using FontKey = std::pair<std::string, int>;

    /**
     * @struct FontKeyHash
     * @brief FontKey类型的哈希函数
     *
     * 为FontKey提供自定义哈希实现，使其能作为std::unordered_map的键值，
     * 哈希值由路径字符串哈希和字号整数哈希异或得到
     */
    struct FontKeyHash
    {
        /**
         * @brief 计算FontKey的哈希值
         *
         * @param key 要计算哈希的FontKey对象
         * @return std::size_t 计算得到的哈希值
         */
        std::size_t operator()(const FontKey &key) const
        {
            std::hash<std::string> string_hasher;
            std::hash<int> int_hasher;
            return string_hasher(key.first) ^ int_hasher(key.second);
        }
    };

    /**
     * @class FontManager
     * @brief 字体资源管理器类
     *
     * 负责SDL_ttf字体的加载、缓存、卸载，使用智能指针+自定义删除器管理字体生命周期，
     * 避免内存泄漏。仅允许ResourceManager作为友元类访问，确保资源管理的唯一性。
     */
    class FontManager
    {
        // 仅允许ResourceManager访问私有接口，封装资源管理逻辑
        friend class ResourceManager;

    private:
        /**
         * @struct SDLFontDeleter
         * @brief TTF_Font智能指针删除器
         *
         * 确保销毁时调用TTF_CloseFont释放字体资源
         */
        struct SDLFontDeleter
        {
            /**
             * @brief 重载()运算符，实现字体资源的释放
             *
             * @param font 要释放的TTF_Font指针
             */
            void operator()(TTF_Font *font) const
            {
                if (font)
                {
                    TTF_CloseFont(font);
                }
            }
        };

        /**
         * @brief 字体缓存映射表
         *
         * 键：FontKey(文件路径+字号)
         * 值：带自定义删除器的TTF_Font智能指针
         */
        std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> fonts_;

    public:
        /**
         * @brief 构造函数：初始化字体管理器
         */
        FontManager();

        /**
         * @brief 析构函数：释放字体管理器资源
         */
        ~FontManager();

        // 禁用拷贝/移动语义
        // 确保字体管理器实例唯一，避免资源重复释放
        /**
         * @brief 禁用拷贝构造
         */
        FontManager(const FontManager &) = delete;

        /**
         * @brief 禁用拷贝赋值
         */
        FontManager &operator=(const FontManager &) = delete;

        /**
         * @brief 禁用移动构造
         */
        FontManager(FontManager &&) = delete;

        /**
         * @brief 禁用移动赋值
         */
        FontManager &operator=(FontManager &&) = delete;

    private:
        /**
         * @brief 加载字体资源（不存在则加载，已存在则返回缓存）
         *
         * @param file_path 字体文件路径（支持TTF/OTF等）
         * @param point_size 字体大小（像素单位）
         * @return 成功返回TTF_Font指针，失败返回nullptr
         */
        TTF_Font *loadFont(std::string_view file_path, int point_size);

        /**
         * @brief 获取已加载的字体（不存在则尝试自动加载）
         *
         * @param file_path 字体文件路径
         * @param point_size 字体大小
         * @return 成功返回TTF_Font指针，失败返回nullptr
         */
        TTF_Font *getFont(std::string_view file_path, int point_size);

        /**
         * @brief 卸载指定路径+字号的字体资源
         *
         * @param file_path 字体文件路径
         * @param point_size 字体大小
         */
        void unloadFont(std::string_view file_path, int point_size);

        /**
         * @brief 清空所有已加载的字体资源
         */
        void clearFonts();
    };
};
