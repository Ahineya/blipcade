//
// Created by Pavlo Yevsehnieiev
//

#ifndef NAVMESH_H
#define NAVMESH_H

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <nlohmann/json.hpp>


namespace blipcade::collision {
    class ConvexPolygon {
    public:
        std::vector<Vector2> vertices;
        std::vector<ConvexPolygon *> neighbors; // Adjacent regions
        Vector2 centroid; // Added centroid

        explicit ConvexPolygon(std::vector<Vector2> verts);

        [[nodiscard]] bool sharesEdge(const ConvexPolygon &other) const;
        [[nodiscard]] std::optional<std::pair<Vector2, Vector2> > getSharedEdge(const ConvexPolygon &other) const;

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

    struct VectorPairHash {
        std::size_t operator()(const std::pair<Vector2, Vector2> &pair) const {
            std::size_t h1 = std::hash<float>()(pair.first.x) ^ std::hash<float>()(pair.first.y);
            std::size_t h2 = std::hash<float>()(pair.second.x) ^ std::hash<float>()(pair.second.y);
            return h1 ^ (h2 << 1); // Combining the hashes
        }

        bool operator()(const std::pair<Vector2, Vector2> &lhs, const std::pair<Vector2, Vector2> &rhs) const {
            return Vector2Equals(lhs.first, rhs.first) && Vector2Equals(lhs.second, rhs.second);
        }
    };

    class NavMesh {
    public:
        NavMesh() = default;

        ~NavMesh() = default;

        std::vector<std::shared_ptr<ConvexPolygon> > regions;

        void addRegion(std::vector<Vector2> verts);

        void buildConnectivity();

        void calculateCentroids();

        void buildOutline();

        [[nodiscard]] bool isLineIntersectingOutline(Vector2 a, Vector2 b, Vector2 &intersection) const;

        [[nodiscard]] const std::vector<std::pair<Vector2, Vector2> > *getOutline() const;

        [[nodiscard]] nlohmann::json toJson() const;


        static NavMesh fromJson(const nlohmann::json &j);

    private:
        std::vector<std::pair<Vector2, Vector2> > outline;
    };
} // collision
// blipcade

#endif //NAVMESH_H
