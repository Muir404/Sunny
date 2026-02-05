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
        // 加载关卡
        engine::scene::LevelLoader level_loader;
        level_loader.loadLevel("assets/maps/level1.tmj", *this);

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

        createTestObject();

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
        TestObject();
        TestCollisionParis();
    }
    void GameScene::clean()
    {
        Scene::clean();
    }
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

    void GameScene::TestCollisionParis()
    {
        auto collision_paris = context_.getPhysicsEngine().getCollisionPairs();
        for (auto &pair : collision_paris)
        {
            spdlog::info("碰撞对：{}和{}", pair.first->getName(), pair.second->getName());
        }
    }
}