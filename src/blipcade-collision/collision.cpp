//
// Created by Pavlo Yevsehnieiev
//

#include "collision.h"

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace blipcade::collision {
    ColliderType colliderTypeFromString(const std::string &type) {
        static const std::unordered_map<std::string, ColliderType> colliderMap = {
            {"rectangle", ColliderType::RECTANGLE},
            {"circle", ColliderType::CIRCLE},
            {"convex_polygon", ColliderType::CONVEX_POLYGON},
            {"concave_polygon", ColliderType::CONCAVE_POLYGON},
            {"point", ColliderType::POINT},
            {"line", ColliderType::LINE},
            {"ray", ColliderType::RAY}
        };

        auto it = colliderMap.find(type);
        if (it != colliderMap.end()) {
            return it->second;
        }

        throw std::invalid_argument("Invalid collider type: " + type);
    }
} // collision
// blipcade
