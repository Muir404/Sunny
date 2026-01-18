#include "game_app.h"
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include "config.h"
#include "../input/input_manager.h"

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

        // time_->setTargetFps(60); 优化为通过配置文件读取信息

        // 初始化正常，开始游戏主循环
        while (is_running_)
        {
            time_->update();
            float delta_time = time_->getDeltaTime();
            input_manager_->update(); // 每帧显更新输入管理器

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

        if (!initConfig())
        {
            return false;
        }

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

        if (!initRenderer())
        {
            return false;
        }

        if (!initCamera())
        {
            return false;
        }

        if (!initInputManager())
        {
            return false;
        }

        testResourceManager();

        is_running_ = true; // 设置为运行状态
        return true;
    }

    void GameApp::handleEvents()
    {
        // SDL_Event event;              // 事件集合
        // while (SDL_PollEvent(&event)) // 对实践进行轮询
        // {
        //     if (event.type == SDL_EVENT_QUIT)
        //     {
        //         is_running_ = false; // 触发离开时间，设置运行标识为结束（假）
        //     }
        // }
        // 转移到input_manager内部处理
        if (input_manager_->shouldQuit())
        {
            spdlog::trace("GameApp收到来自InputManager的退出请求");
            is_running_ = false;
            return;
        }

        testInputManager();
    }

    void GameApp::update(float /*delta_time*/)
    {
        // TODO 游戏逻辑更新
        testCamera();
    }

    void GameApp::render()
    {
        // TODO 渲染代码
        // 1. clear the screen
        renderer_->clearScreen();

        // 2. render
        testRenderer();

        // 3. update the screen
        renderer_->present();
    }

    void GameApp::close()
    {
        spdlog::trace("关闭GameApp中");

        // 为了确保正确的销毁顺序，有些智能指针对象也需要手动管理
        resource_manager_.reset();
        time_.reset();

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

    bool GameApp::initConfig()
    {
        try
        {
            config_ = std::make_unique<engine::core::Config>("assets/config.json");
        }
        catch (const std::exception &e)
        {
            spdlog::error("初始化配置失败：{}", e.what());
            return false;
        }
        spdlog::trace("配置初始化成功");
        return true;
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) // 先看看SDL是否启动成功
        {
            spdlog::error("SDL初始化失败！SDL错误：{}", SDL_GetError());
            return false;
        }

        window_ = SDL_CreateWindow(config_->window_title_.c_str(), config_->window_width_, config_->window_height_, SDL_WINDOW_RESIZABLE); // 尝试启动window_
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

        int vsync_mode = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
        SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
        spdlog::trace("VSync设置为：{}", config_->vsync_enabled_ ? "Enable" : "Disable");

        SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_ / 2, config_->window_height_ / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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

        time_->setTargetFps(config_->target_fps_);
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

    bool GameApp::initRenderer()
    {
        try
        {
            renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_, resource_manager_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("初始化渲染器失败：{}", e.what());
            return false;
        }
        spdlog::trace("渲染器初始化成功");
        return true;
    }

    bool GameApp::initCamera()
    {
        try
        {
            camera_ = std::make_unique<engine::render::Camera>(glm::vec2(config_->window_width_ / 2, config_->window_height_ / 2));
        }
        catch (const std::exception &e)
        {
            spdlog::error("初始化相机失败：{}", e.what());
            return false;
        }
        spdlog::trace("相机初始化成功");
        return true;
    }

    bool GameApp::initInputManager()
    {
        try
        {
            input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::trace("输入管理器初始化失败", e.what());
        }
        spdlog::trace("输入管理器初始化成功");
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

    void GameApp::testRenderer()
    {
        // 精灵对象定义
        engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
        engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
        engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

        // 旋转变量
        static float rotation = 0.0f;
        rotation += 0.1f;

        // 渲染调用 (注意渲染顺序)
        renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100, 100), glm::vec2(0.5f, 0.5f), glm::bvec2{true, true});
        renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200, 200), glm::vec2(1.0f, 1.0f), rotation);
        renderer_->drawUISprite(sprite_ui, glm::vec2(100, 100));
    }

    void GameApp::testCamera()
    {
        auto key_state = SDL_GetKeyboardState(nullptr);
        if (key_state[SDL_SCANCODE_UP])
            camera_->move(glm::vec2(0, -1));
        if (key_state[SDL_SCANCODE_DOWN])
            camera_->move(glm::vec2(0, 1));
        if (key_state[SDL_SCANCODE_LEFT])
            camera_->move(glm::vec2(-1, 0));
        if (key_state[SDL_SCANCODE_RIGHT])
            camera_->move(glm::vec2(1, 0));
    }
    void GameApp::testInputManager()
    {
        std::vector<std::string> actions = {
            "move_up",
            "move_down",
            "move_left",
            "move_right",
            "jump",
            "attack",
            "pause",
            "MouseLeftClick",
            "MouseRightClick"};
        for (const auto &action : actions)
        {
            if (input_manager_->isActionPressed(action))
            {
                spdlog::info("{} 按下 ", action);
            }
            if (input_manager_->isActionReleased(action))
            {
                spdlog::info("{} 抬起 ", action);
            }
            if (input_manager_->isActionDown(action))
            {
                spdlog::info("{} 按下 中 ", action);
            }
        }
    }
}