//
// Created by Pavlo Yevsehnieiev
//

#ifndef COLLIDER_H
#define COLLIDER_H
#include <collision.h>
#include <raylib.h>
#include <nlohmann/json.hpp>

namespace blipcade::collision {
    enum class ColliderType;

    class Collider {
    public:
        Collider(ColliderType type, std::vector<Vector2> vertices);

        ~Collider() = default;

        static Collider fromJson(const nlohmann::json &colliderJson);

        const ColliderType type;
        std::vector<Vector2> vertices;
        std::vector<Triangle> triangles{};

    private:
    };
} // collision
// blipcade

#endif //COLLIDER_H