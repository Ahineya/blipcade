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
        Vector2 centroid; // Added centroid

        ConvexPolygon(std::vector<Vector2> verts);

        bool sharesEdge(const ConvexPolygon& other) const;

        std::optional<std::pair<Vector2, Vector2>> getSharedEdge(const ConvexPolygon &other) const;

        void calculateCentroid() {
            float signedArea = 0;
            float cx = 0;
            float cy = 0;
            size_t count = vertices.size();
            for (size_t i = 0; i < count; ++i) {
                size_t j = (i + 1) % count;
                float a = vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
                signedArea += a;
                cx += (vertices[i].x + vertices[j].x) * a;
                cy += (vertices[i].y + vertices[j].y) * a;
            }
            signedArea *= 0.5f;
            cx /= (6.0f * signedArea);
            cy /= (6.0f * signedArea);
            centroid = Vector2{cx, cy};
        }
    };

    class NavMesh {
    public:
        NavMesh() = default;
        ~NavMesh() = default;
        std::vector<std::shared_ptr<ConvexPolygon>> regions;

        void addRegion(std::vector<Vector2> verts);

        void buildConnectivity();
        void calculateCentroids();

        nlohmann::json toJson() const;

        static NavMesh fromJson(const nlohmann::json &j);
    };

} // collision
} // blipcade

#endif //NAVMESH_H
