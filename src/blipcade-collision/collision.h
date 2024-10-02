//
// Created by Pavlo Yevsehnieiev
//

#ifndef COLLISION_H
#define COLLISION_H
#include <raylib.h>
#include <string>


namespace blipcade::collision {

    const enum class ColliderType {
        RECTANGLE,
        CIRCLE,
        CONVEX_POLYGON,
        CONCAVE_POLYGON,
        POINT,
        LINE,
        RAY,
    };

    ColliderType colliderTypeFromString(const std::string &type);

    class Triangle {
    public:
        Triangle(Vector2 v1, Vector2 v2, Vector2 v3): v1(v1), v2(v2), v3(v3) {}
        ~Triangle() = default;
        Vector2 v1;
        Vector2 v2;
        Vector2 v3;
    };

} // collision
// blipcade

#endif //COLLISION_H
