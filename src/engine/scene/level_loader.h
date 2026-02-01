#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <optional>
#include "../utils/math.h"

namespace engine::scene
{
    class Scene;

    class LevelLoader final
    {
        std::string map_path_; // 地图路径
    public:
        LevelLoader() = default;

        bool loadLevel(const std::string &map_path, Scene &scene);

    private:
        void loadImageLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadTileLayer(const nlohmann::json &layer_json, Scene &scene);
        void loadObjectLayer(const nlohmann::json &layer_json, Scene &scene);

        /**
         * @brief 解析图片路径，合并地图路径和相对路径。例如：
         * 1. 文件路径："assets/maps/level1.tmj"
         * 2. 相对路径："../textures/Layers/back.png"
         * 3. 最终路径："assets/textures/Layers/back.png"
         * @param relative_path 相对路径（相对于文件）
         * @param file_path 文件路径
         * @return std::string 解析后的完整路径。
         */
        std::string resolvePath(std::string image_path);
    };

} // namespace engine::scene