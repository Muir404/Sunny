#include "resource_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include "font_manager.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "spdlog/spdlog.h"

namespace engine::resource
{
    ResourceManager::ResourceManager(SDL_Renderer *renderer)
    {
        texture_manager_ = std::make_unique<TextureManager>(renderer);
        audio_manager_ = std::make_unique<AudioManager>();
        font_manager_ = std::make_unique<FontManager>();

        spdlog::trace("资源管理类构造成功");
    }

    void ResourceManager::clear()
    {
        font_manager_->clearFonts();
        audio_manager_->clearAudio();
        texture_manager_->clearTextures();
        spdlog::trace("已通过资源管理器中的clear()清理资源");
    }

    ResourceManager::~ResourceManager() = default;

    // 纹理接口
    SDL_Texture *ResourceManager::loadTexture(const std::string &file_path)
    {
        return texture_manager_->loadTexture(file_path);
    }

    SDL_Texture *ResourceManager::getTexture(const std::string &file_path)
    {
        return texture_manager_->getTexture(file_path);
    }
    void ResourceManager::unloadTexture(const std::string &file_path)
    {
        texture_manager_->unloadTexture(file_path);
    }
    glm::vec2 ResourceManager::getTextureSize(const std::string &file_path)
    {
        return texture_manager_->getTextureSize(file_path);
    }
    void ResourceManager::clearTextures()
    {
        texture_manager_->clearTextures();
    }

    // 音效接口
    MIX_Audio *ResourceManager::loadSound(const std::string &file_path)
    {
        return audio_manager_->loadSound(file_path);
    }
    MIX_Audio *ResourceManager::getSound(const std::string &file_path)
    {
        return audio_manager_->getSound(file_path);
    }
    void ResourceManager::unloadSound(const std::string &file_path)
    {
        audio_manager_->unloadSound(file_path);
    }
    void ResourceManager::clearSounds()
    {
        audio_manager_->clearSounds();
    }

    // 音乐接口
    MIX_Audio *ResourceManager::loadMusic(const std::string &file_path)
    {
        return audio_manager_->loadMusic(file_path);
    }
    MIX_Audio *ResourceManager::getMusic(const std::string &file_path)
    {
        return audio_manager_->getMusic(file_path);
    }
    void ResourceManager::unloadMusic(const std::string &file_path)
    {
        audio_manager_->unloadMusic(file_path);
    }
    void ResourceManager::clearMusic()
    {
        audio_manager_->clearMusic();
    }

    // 字体接口
    TTF_Font *ResourceManager::loadFont(std::string file_path, int point_size)
    {
        return font_manager_->loadFont(file_path, point_size);
    }
    TTF_Font *ResourceManager::getFont(std::string file_path, int point_size)
    {
        return font_manager_->getFont(file_path, point_size);
    }
    void ResourceManager::unloadFont(std::string file_path, int point_size)
    {
        font_manager_->unloadFont(file_path, point_size);
    }
    void ResourceManager::clearFonts()
    {
        font_manager_->clearFonts();
    }
};