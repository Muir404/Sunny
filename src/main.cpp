#include "engine/core/game_app.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_main.h>

int main(int /*argc*/, char * /*argv*/[])
{
    spdlog::set_level(spdlog::level::off);
    spdlog::info("游戏开始");

    engine::core::GameApp app;
    app.run();
    return 0;
}