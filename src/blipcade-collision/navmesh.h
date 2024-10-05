//
// Created by Pavlo Yevsehnieiev
//

#ifndef NAVMESH_H
#define NAVMESH_H

#include <iostream>
#include <raylib.h>
#include <vector>
#include <nlohmann/json.hpp>


namespace blipcade {
namespace collision {

    class ConvexPolygon {
    public:
        std::vector<Vector2> vertices;
        std::vector<ConvexPolygon*> neighbors; // Adjacent regions

        ConvexPolygon(std::vector<Vector2> verts);

        bool sharesEdge(const ConvexPolygon& other) const;
    };

    class NavMesh {
    public:
        std::vector<std::unique_ptr<ConvexPolygon>> regions;


        void addRegion(std::vector<Vector2> verts);

        void buildConnectivity();

        nlohmann::json toJson() const;

        NavMesh fromJson(const nlohmann::json &j);
    };

} // collision
} // blipcade

#endif //NAVMESH_H
