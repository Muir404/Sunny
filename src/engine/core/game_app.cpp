#include "game_app.h"
#include "time.h"
#include "../resource/resource_manager.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

namespace engine::core // 命名空间与路径一致
{
    GameApp::GameApp() = default;

    GameApp::~GameApp()
    {
        if (is_running_)
        {
            spdlog::warn("GameApp被销毁时没有显式关闭。现在关闭中……");
            close();
        }
    }

    void GameApp::run()
    {
        if (!init()) // 初始化失败
        {
            spdlog::error("初始化失败，无法运行游戏");
            return;
        }

        time_->setTargetFps(60);

        // 初始化正常，开始游戏主循环
        while (is_running_)
        {
            time_->update();
            float delta_time = time_->getDeltaTime();

            handleEvents();
            update(delta_time);
            render();

            // spdlog::info("delta_time：{}", delta_time);
        }
        close(); // 离开游戏则清理
    }

    bool GameApp::init()
    {
        spdlog::trace("初始化GameApp……");

        if (!initSDL())
        {
            return false;
        }

        if (!initTime())
        {
            return false;
        }

        if (!initResourceManager())
        {
            return false;
        }

        testResourceManager();

        is_running_ = true; // 设置为运行状态
        return true;
    }

    void GameApp::handleEvents()
    {
        SDL_Event event;              // 事件集合
        while (SDL_PollEvent(&event)) // 对实践进行轮询
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running_ = false; // 触发离开时间，设置运行标识为结束（假）
            }
        }
    }

    void GameApp::update(float /*delta_time*/)
    {
        // TODO 游戏逻辑更新
    }

    void GameApp::render()
    {
        // TODO 渲染代码
    }

    void GameApp::close()
    {
        spdlog::trace("关闭GameApp中");

        // 为了确保正确的销毁顺序，有些智能指针对象也需要手动管理
        resource_manager_.reset();

        if (sdl_renderer_ != nullptr)
        {
            SDL_DestroyRenderer(sdl_renderer_);
            sdl_renderer_ = nullptr;
        }

        if (window_ != nullptr)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

        is_running_ = false;
        SDL_Quit();
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) // 先看看SDL是否启动成功
        {
            spdlog::error("SDL初始化失败！SDL错误：{}", SDL_GetError());
            return false;
        }

        window_ = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE); // 尝试启动window_
        if (window_ == nullptr)
        {
            spdlog::error("无法创建窗口！SDL错误：{}", SDL_GetError());
            return false;
        }

        sdl_renderer_ = SDL_CreateRenderer(window_, nullptr); // 尝试把渲染器放入窗口，不限制渲染器(opengl\vulkan)
        if (sdl_renderer_ == nullptr)
        {
            spdlog::error("无法创建渲染器！SDL错误：{}", SDL_GetError());
            return false;
        }
        spdlog::trace("SDL初始化成功");
        return true;
    }

    bool GameApp::initTime()
    {
        try
        {
            time_ = std::make_unique<Time>();
        }
        catch (const std::exception &e)
        {
            spdlog::error("初始化时间管理失败: {}", e.what());
            return false;
        }
        spdlog::trace("时间管理初始化成功。");
        return true;
    }
    bool GameApp::initResourceManager()
    {
        try
        {
            resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("初始化资源管理器失败: {}", e.what());
            return false;
        }
        spdlog::trace("资源管理器初始化成功。");
        return true;
    }
    void GameApp::testResourceManager()
    {
        resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png"); // 加载纹理资源
        resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);    // 加载字体资源（带字号参数）
        resource_manager_->getSound("assets/audio/button_click.wav");             // 加载音频资源

        resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png"); // 卸载纹理资源
        resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);    // 卸载字体资源
        resource_manager_->unloadSound("assets/audio/button_click.wav");             // 卸载音频资源
    }
}