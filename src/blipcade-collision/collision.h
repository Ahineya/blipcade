//
// Created by Pavlo Yevsehnieiev
//

#ifndef COLLISION_H
#define COLLISION_H
#include <ostream>
#include <raylib.h>
#include <string>


namespace blipcade::collision {
    enum class ColliderType {
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

        // Add a friend function to pretty print the triangle
        friend std::ostream &operator<<(std::ostream &os, const Triangle &triangle) {
            os << "Triangle: " << std::endl;
            os << "v1: " << triangle.v1.x << ", " << triangle.v1.y << std::endl;
            os << "v2: " << triangle.v2.x << ", " << triangle.v2.y << std::endl;
            os << "v3: " << triangle.v3.x << ", " << triangle.v3.y << std::endl;
            return os;
        }
    };


} // collision
// blipcade

#endif //COLLISION_H
