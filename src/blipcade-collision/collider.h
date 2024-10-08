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
        static Collider fromResource(const std::string &resourcePath, const std::string &projectDir);

        const ColliderType type;
        std::vector<Vector2> vertices;
        std::vector<Triangle> triangles{};

        bool checkCollisionPoint(const Vector2& point) const;

    private:
        float computeSignedArea() const;
    };
} // collision
// blipcade

#endif //COLLIDER_H
