#pragma once
#include <string>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

namespace engine::core
{
    class Config;
}

namespace engine::input
{

    enum class ActionState
    {
        INACTIVE,           // 动作未激活
        PRESSED_THIS_FRAME, // 动作在本帧刚刚被按下
        HELD_DOWN,          // 动作被持续按下
        RELEASED_THIS_FRAME // 动作在本帧刚刚被释放
    };

    class InputManager final
    {
    private:
        SDL_Renderer *sdl_renderer_; // 用于获取逻辑坐标的 SDL_Renderer 指针

        std::unordered_map<std::string, std::vector<std::string>> actions_to_keyname_map_;                      // 存储动作名称到按键名称列表的映射
        std::unordered_map<std::variant<Uint32, SDL_Scancode>, std::vector<std::string>> input_to_actions_map_; // 从输入到关联的动作名称列表
        std::unordered_map<std::string, ActionState> action_states_;                                            // 存储每个动作的当前状态

        bool should_quit_ = false; // 退出标志
        glm::vec2 mouse_position_; // 鼠标位置 (针对屏幕坐标)

    public:
        InputManager(SDL_Renderer *sdl_renderer, const engine::core::Config *config);

        void update(); // 更新输入状态，每轮循环最先调用

        // 动作状态检查
        bool isActionDown(std::string action_name) const;     // 动作当前是否触发 (持续按下或本帧按下)
        bool isActionPressed(std::string action_name) const;  // 动作是否在本帧刚刚按下
        bool isActionReleased(std::string action_name) const; // 动作是否在本帧刚刚释放

        bool shouldQuit() const;              // 查询退出状态
        void setShouldQuit(bool should_quit); // 设置退出状态

        glm::vec2 getMousePosition() const;        // 获取鼠标位置 （屏幕坐标）
        glm::vec2 getLogicalMousePosition() const; // 获取鼠标位置 （逻辑坐标）

    private:
        void processEvent(const SDL_Event &event);                   // 处理 SDL 事件（将按键转换为动作状态）
        void initializeMappings(const engine::core::Config *config); // 根据 Config配置初始化映射表

        void updateActionState(std::string action_name, bool is_input_active, bool is_repeat_event); // 辅助更新动作状态
        SDL_Scancode scancodeFromString(std::string key_name);                                       // 将字符串键名转换为 SDL_Scancode
        Uint32 mouseButtonUint32FromString(std::string button_name);                                 // 将字符串按钮名转换为 SDL_Button
    };

} // namespace engine::input