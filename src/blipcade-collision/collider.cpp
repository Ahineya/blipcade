//
// Created by Pavlo Yevsehnieiev
//

// #include "collider.h"

#include <collider.h>
#include <collision.h>
#include <raylib.h>
#include <string>
#include <nlohmann/json_fwd.hpp>


namespace blipcade::collision {
    Collider::Collider(ColliderType type, std::vector<Vector2> vertices): type(type), vertices(std::move(vertices)) {
    }

    Collider Collider::fromJson(const nlohmann::json &colliderJson) {

        auto const strType = colliderJson["type"].get<std::string>();
        auto const type = colliderTypeFromString(strType);

        std::vector<Vector2> vertices;
        for (const auto &vertexJson : colliderJson["vertices"]) {
            vertices.push_back({vertexJson["x"].get<float>(), vertexJson["y"].get<float>()});
        }

        return {type, vertices};
    }
} // collision
// blipcade