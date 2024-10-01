//
// Created by Pavlo Yevsehnieiev
//

#ifndef COLLISION_H
#define COLLISION_H
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

} // collision
// blipcade

#endif //COLLISION_H
