// ==============================
// 标准库头文件
// ==============================
#include <stdexcept>
#include <string>

// ==============================
// 其它文件引入
// ==============================
#include "font_manager.h"

// ==============================
// 第三方库头文件
// ==============================
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

// ==============================
// 命名空间与实现
// ==============================
namespace engine::resource
{
    /**
     * @brief 字体管理器的构造函数
     *
     * 初始化SDL_ttf库，若初始化失败则抛出运行时异常。
     *
     * @throw std::runtime_error TTF_Init初始化失败时抛出异常
     */
    FontManager::FontManager()
    {
        if (!TTF_WasInit() && !TTF_Init())
        {
            throw std::runtime_error("字体管理器错误: TTF_Init 失败：" + std::string(SDL_GetError()));
        }
        spdlog::trace("FontManager: 构造成功");
    }

    /**
     * @brief 字体管理器的析构函数
     *
     * 清理所有缓存的字体资源，退出SDL_ttf库，确保资源完全释放。
     */
    FontManager::~FontManager()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("FontManager: 缓存不为空，执行字体清理逻辑");
            clearFonts(); // 调用 clearFonts 处理清理逻辑
        }
        TTF_Quit();
        spdlog::trace("FontManager: 析构成功");
    }

    /**
     * @brief 加载字体资源（不存在则加载，已存在则返回缓存）
     *
     * @param file_path 字体文件路径（支持TTF/OTF等格式）
     * @param point_size 字体大小（像素单位，必须大于0）
     * @return 成功返回TTF_Font指针，失败返回nullptr
     */
    TTF_Font *FontManager::loadFont(std::string_view file_path, int point_size)
    {
        // 检查点大小是否有效
        if (point_size <= 0)
        {
            spdlog::error("FontManager: 无法加载字体 '{}'：无效的字号 {}", file_path, point_size);
            return nullptr;
        }

        // 创建映射表的键
        FontKey key = {std::string(file_path), point_size};

        // 首先检查缓存
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }

        // 缓存中不存在，则加载字体
        spdlog::debug("FontManager: 正在加载字体：{} ({}pt)", file_path, point_size);
        TTF_Font *raw_font = TTF_OpenFont(file_path.data(), point_size);
        if (!raw_font)
        {
            spdlog::error("FontManager: 加载字体 '{}' ({}pt) 失败：{}", file_path, point_size, SDL_GetError());
            return nullptr;
        }

        // 使用 unique_ptr 存储到缓存中
        fonts_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(raw_font));
        spdlog::debug("FontManager: 成功加载并缓存字体：{} ({}pt)", file_path, point_size);
        return raw_font;
    }

    /**
     * @brief 获取已加载的字体（不存在则尝试自动加载）
     *
     * @param file_path 字体文件路径
     * @param point_size 字体大小
     * @return 成功返回TTF_Font指针，失败返回nullptr
     */
    TTF_Font *FontManager::getFont(std::string_view file_path, int point_size)
    {
        FontKey key = {std::string(file_path), point_size};
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }

        spdlog::warn("FontManager: 字体 '{}' ({}pt) 不在缓存中，尝试加载", file_path, point_size);
        return loadFont(file_path, point_size);
    }

    /**
     * @brief 卸载指定路径+字号的字体资源
     *
     * @param file_path 字体文件路径
     * @param point_size 字体大小
     */
    void FontManager::unloadFont(std::string_view file_path, int point_size)
    {
        FontKey key = {std::string(file_path), point_size};
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            spdlog::debug("FontManager: 卸载字体：{} ({}pt)", file_path, point_size);
            fonts_.erase(it); // unique_ptr 会处理 TTF_CloseFont
        }
        else
        {
            spdlog::warn("FontManager: 尝试卸载不存在的字体：{} ({}pt)", file_path, point_size);
        }
    }

    /**
     * @brief 清空所有已加载的字体资源
     *
     * 通过清空unordered_map触发unique_ptr的析构，自动调用TTF_CloseFont释放资源
     */
    void FontManager::clearFonts()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("FontManager: 正在清理所有 {} 个缓存的字体", fonts_.size());
            fonts_.clear(); // unique_ptr 会处理删除
        }
    }
}
