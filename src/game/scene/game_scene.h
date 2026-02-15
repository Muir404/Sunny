#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <glm/vec2.hpp>

// 前置声明
namespace engine::object
{
    class GameObject;
}

namespace game::data
{
    class SessionData;
}

namespace engine::ui
{
    class UILabel;
    class UIPanel;
}

namespace game::scene
{
    /**
     * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等。
     */
    class GameScene final : public engine::scene::Scene
    {
        std::shared_ptr<game::data::SessionData> game_session_data_;
        engine::object::GameObject *player_ = nullptr;

        engine::ui::UILabel *score_label_ = nullptr;  ///< @brief 得分标签 (生命周期由UIManager管理，因此使用裸指针)
        engine::ui::UIPanel *health_panel_ = nullptr; ///< @brief 生命值图标面板

    public:
        GameScene(engine::core::Context &context,
                  engine::scene::SceneManager &scene_manager,
                  std::shared_ptr<game::data::SessionData> data = nullptr);

        // 覆盖场景基类的核心方法
        void init() override;
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;
        void clean() override;

    private:
        [[nodiscard]] bool initLevel();
        [[nodiscard]] bool initPlayer();
        [[nodiscard]] bool initEnemyAndItem();
        [[nodiscard]] bool initUI();

        void handleObjectCollisons();                                                                       ///< @brief 处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息）
        void handleTileTriggers();                                                                          ///< @brief 处理瓦片触发事件（从PhysicsEngine获取信息）
        void handlePlayerDamage(int damage);                                                                // 造成伤害
        void playerVSEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy); ///< @brief 玩家与敌人碰撞处理
        void playerVSItemCollision(engine::object::GameObject *player, engine::object::GameObject *item);   ///< @brief 玩家与道具碰撞处理

        void toNextLevel(engine::object::GameObject *trigger); // 进入下一关
        void showEndScene(bool is_win);                        // 显示结束场景

        std::string levelNameToPath(const std::string &level_name) const { return "assets/maps/" + level_name + ".tmj"; } /// @brief 根据关卡名称获取对应的地图文件路径

        /**
         * @brief 创建一个特效对象（一次性）。
         * @param center_pos 特效中心位置
         * @param tag 特效标签（决定特效类型,例如"enemy","item"）
         */
        void createEffect(glm::vec2 center_pos, std::string_view tag);

        // UI
        void createScoreUI();           // 得分UI
        void createHealthUI();          // 生命值UI
        void addScoreWithUI(int score); // 增加得分，更新UI
        void healWithUI(int amount);    // 增加生命，更新UI
        void updateHealthWithUI();      // 更新生命UI

        // void createTestButtion();
        // void testButtonClicked();
        // void testSaveAndLoad();
        // void testTextRenderer();
        // void createTestObject();
        // void testCamera();
        // void TestObject();
        // void TestPlayer();
        // void TestCollisionParis();
        // void testHealth();
    };

} // namespace game::scene