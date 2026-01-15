#include "game_app.h"

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
        // 初始化正常，开始游戏主循环
        while (is_running_)
        {
            float delta_time = 0.01f;
            handleEvents();
            update(delta_time);
            render();
        }
        close(); // 离开游戏则清理
    }

    bool GameApp::init()
    {
        spdlog::trace("初始化GameApp……");
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
        SDL_Quit();
        is_running_ = false;
    }
}