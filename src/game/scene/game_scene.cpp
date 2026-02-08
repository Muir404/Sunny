#include "../../engine/core/context.h"

#include "../../engine/object/game_object.h"

#include "../../engine/input/input_manager.h"

#include "../../engine/scene/scene_manager.h"
#include "../../engine/scene/level_loader.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"

#include "../../engine/render/camera.h"

#include "../../engine/physics/physics_engine.h"
#include "../../engine/physics/collision.h"

#include "../../engine/utils/math.h"

#include "../component/player_component.h"

#include "game_scene.h"

#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

namespace game::scene
{
    GameScene::GameScene(std::string name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
        : Scene(name, context, scene_manager)
    {
        spdlog::trace("HelpsScene 创建.");
    }
    void GameScene::init()
    {
        if (!initLevel())
        {
            spdlog::error("关卡初始化失败，无法继续");
            context_.getInputManager().setShouldQuit(true);
            return;
        }

        if (!initPlayer())
        {
            spdlog::error("玩家初始化失败，无法继续");
            context_.getInputManager().setShouldQuit(true);
            return;
        }

        if (!initEnemyAndItem())
        {
            spdlog::error("敌人和道具初始化失败，无法继续");
            context_.getInputManager().setShouldQuit(true);
            return;
        }

        Scene::init();
        spdlog::trace("GameScene初始化完成");
    }

    void GameScene::update(float delta_time)
    {
        Scene::update(delta_time);
    }

    void GameScene::render()
    {
        Scene::render();
    }

    void GameScene::handleInput()
    {
        Scene::handleInput();
        // testCamera();
        // TestObject();
        // TestPlayer();
        // TestCollisionParis();
    }

    void GameScene::clean()
    {
        Scene::clean();
    }

    bool GameScene::initLevel()
    {
        // 加载关卡
        engine::scene::LevelLoader level_loader;
        if (!level_loader.loadLevel("assets/maps/level1.tmj", *this))
        {
            spdlog::error("关卡加载失败");
            return false;
        }

        // 注册 main 到物理引擎
        auto *main_layer = findGameObjectByName("main");
        if (main_layer)
        {
            auto *tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
            if (tile_layer)
            {
                context_.getPhysicsEngine().registerCollisionLayer(tile_layer);
                spdlog::info("注册\"main\"到物理引擎");
            }
        }

        // 设置相机边界
        auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
        context_.getCamera().setLimitBounds(engine::utils::Rect{glm::vec2(0.0f), world_size});

        // 设置世界边界
        context_.getPhysicsEngine().setWorldBounds(engine::utils::Rect{glm::vec2(0.0f), world_size});

        spdlog::trace("关卡初始化成功");
        return true;
    }

    bool GameScene::initPlayer()
    {
        player_ = findGameObjectByName("player");
        if (!player_)
        {
            spdlog::error("未找到玩家对象");
            return false;
        }
        // 添加playercomponent到玩家对象
        auto *player_component = player_->addComponent<game::component::PlayerComponent>();
        if (!player_component)
        {
            spdlog::error("无法添加PlayerComponent到玩家对象");
            return false;
        }
        // 相机跟随
        auto *player_transform = player_->getComponent<engine::component::TransformComponent>();
        if (!player_transform)
        {
            spdlog::error("玩家没有Transform组件，无法设置相机");
            return false;
        }

        context_.getCamera().setTarget(player_transform);

        spdlog::trace("Player初始化完成");
        return true;
    }

    bool GameScene::initEnemyAndItem()
    {
        bool success = true;
        /*
        for (auto &game_object : game_objects_)
        {
            if (game_object->getName() == "eagle")
            {
                if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
                {
                    auto y_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().y;
                    auto y_min = y_max - 80.0f; // 让鹰的飞行范围 (当前位置与上方80像素 的区域)
                    ai_component->setBehavior(std::make_unique<game::component::ai::UpDownBehavior>(y_min, y_max));
                }
            }
            if (game_object->getName() == "frog")
            {
                if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
                {
                    auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                    auto x_min = x_max - 90.0f; // 青蛙跳跃范围（右侧 - 10.0f 是为了增加稳定性）
                    ai_component->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(x_min, x_max));
                }
            }
            if (game_object->getName() == "opossum")
            {
                if (auto *ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component)
                {
                    auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                    auto x_min = x_max - 200.0f; // 负鼠巡逻范围
                    ai_component->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(x_min, x_max));
                }
            }
            if (game_object->getTag() == "item")
            {
                if (auto *ac = game_object->getComponent<engine::component::AnimationComponent>(); ac)
                {
                    ac->playAnimation("idle");
                }
                else
                {
                    spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
                    success = false;
                }
            }
        }
        */
        return success;
    }

    /*
    void GameScene::createTestObject()
    {
        spdlog::trace("在 GameScene 中创建 test_object...");

        auto test_object = std::make_unique<engine::object::GameObject>("test_object");
        test_object_ = test_object.get();
        // 添加组件
        test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f, 100.0f));
        test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", context_.getResourceManager());
        test_object->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine());
        test_object->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2{32.0f, 32.0f}));

        // 将创建好的 GameObject 添加到场景中 （一定要用 std::move，否则传递的是左值）
        addGameObject(std::move(test_object));

        // 添加第二个组件
        auto test_object2 = std::make_unique<engine::object::GameObject>("test_object2");
        test_object2->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f, 100.0f));
        test_object2->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", context_.getResourceManager());
        test_object2->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), false);
        test_object2->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2{32.0f, 32.0f}));
        addGameObject(std::move(test_object2));

        spdlog::trace("test_object 创建并添加到 GameScene 中.");
    }

    void GameScene::testCamera()
    {
        auto &camera = context_.getCamera();
        auto &input_manager = context_.getInputManager();
        if (input_manager.isActionDown("move_up"))
        {
            camera.move(glm::vec2(0, -1));
        }
        if (input_manager.isActionDown("move_down"))
        {
            camera.move(glm::vec2(0, 1));
        }
        if (input_manager.isActionDown("move_left"))
        {
            camera.move(glm::vec2(-1, 0));
        }
        if (input_manager.isActionDown("move_right"))
        {
            camera.move(glm::vec2(1, 0));
        }
    }

    void GameScene::TestObject()
    {
        if (!test_object_)
            return;
        auto &input_manager = context_.getInputManager();
        auto *pc = test_object_->getComponent<engine::component::PhysicsComponent>();
        if (!pc)
            return;

        if (input_manager.isActionDown("move_left"))
        {
            pc->velocity_.x = -100.0f;
        }
        else
        {
            pc->velocity_.x *= 0.9f;
        }

        if (input_manager.isActionDown("move_right"))
        {
            pc->velocity_.x = 100.0f;
        }
        else
        {
            pc->velocity_.x *= 0.9f;
        }

        if (input_manager.isActionPressed("jump"))
        {
            pc->velocity_.y = -400.0f;
        }
    }

    void GameScene::TestPlayer()
    {
        if (!player_)
            return;
        auto &input_manager = context_.getInputManager();
        auto *pc = player_->getComponent<engine::component::PhysicsComponent>();
        if (!pc)
            return;

        if (input_manager.isActionDown("move_left"))
        {
            pc->velocity_.x = -100.0f;
        }
        else
        {
            pc->velocity_.x *= 0.9f;
        }

        if (input_manager.isActionDown("move_right"))
        {
            pc->velocity_.x = 100.0f;
        }
        else
        {
            pc->velocity_.x *= 0.9f;
        }

        if (input_manager.isActionPressed("jump"))
        {
            pc->velocity_.y = -400.0f;
        }
    }

    void GameScene::TestCollisionParis()
    {
        auto collision_paris = context_.getPhysicsEngine().getCollisionPairs();
        for (auto &pair : collision_paris)
        {
            spdlog::info("碰撞对：{}和{}", pair.first->getName(), pair.second->getName());
        }
    }
    */
}