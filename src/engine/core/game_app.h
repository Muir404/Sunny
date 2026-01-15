#pragma once
#include <memory>

// 前向声明，减少头文件依赖，加速编译
struct SDL_Window;
struct SDL_Renderer;

namespace engine::core // 命名空间与路径一致
{
    class Time;

    class GameApp final
    {
    private:
        SDL_Window *window_ = nullptr;
        SDL_Renderer *sdl_renderer_ = nullptr;
        bool is_running_ = false;

        // 引擎组件
        std::unique_ptr<engine::core::Time> time_;

    public:
        GameApp();
        ~GameApp();

        void run();

        // 由于不需要移动构造和复制构造等内容，此处做delete处理
        GameApp(const GameApp &) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp(const GameApp &&) = delete;
        GameApp &operator=(const GameApp &&) = delete;

    private:
        [[nodiscard]] bool init(); // discard表示不可忽略，必须处理
        void handleEvents();
        void update(float delta_time);
        void render();
        void close();
    };
} // namespace engine::core