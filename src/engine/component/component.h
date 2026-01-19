#pragma once
// 前置声明
namespace engine::object
{
    class GameObject;
}

namespace engine::component
{
    class Component
    {
        friend class engine::object::GameObject; // 它需要调用Component的init方法

    protected:
        engine::object::GameObject *owner_ = nullptr; // 指向拥有此组件的 GameObject

    public:
        Component() = default;
        virtual ~Component() = default; // 虚析构函数确保正确清理派生类

        // 禁止拷贝和移动，组件通常不应被拷贝或移动（更改owner_就相当于移动）
        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;
        Component(Component &&) = delete;
        Component &operator=(Component &&) = delete;

        void setOwner(engine::object::GameObject *owner) { owner_ = owner; } // 设置拥有此组件的 GameObject
        engine::object::GameObject *getOwner() const { return owner_; }      // 获取拥有此组件的 GameObject

    protected:
        // 关键循环函数，全部设为保护，只有 GameObject 需要（可以）调用
        virtual void init() {}         // 保留两段初始化的机制，GameObject 添加组件时自动调用，不需要外部调用
        virtual void handleInput() {}  // 处理输入
        virtual void update(float) {}; // 更新，必须实现
        virtual void render() {}       // 渲染
        virtual void clean() {}        // 清理
    };

} // namespace engine::component