//
// Created by Pavlo Yevsehnieiev
//

#ifndef COLLIDER_H
#define COLLIDER_H
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

    private:
        std::vector<Vector2> vertices;
};

} // collision
// blipcade

#endif //COLLIDER_H
