// pathfinding.h

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "navmesh.h"
#include <vector>
#include <optional>

namespace blipcade {
    namespace collision {

        struct PathNode {
            ConvexPolygon* region;
            float gCost; // Cost from start
            float hCost; // Heuristic cost to end
            float fCost() const { return gCost + hCost; }

            PathNode(ConvexPolygon* r, float g, float h, ConvexPolygon* p = nullptr)
                : region(r), gCost(g), hCost(h) {}
        };

        // Comparator for the priority queue
        struct ComparePathNode {
            bool operator()(const PathNode& a, const PathNode& b) const {
                return a.fCost() > b.fCost();
            }
        };

        class Pathfinding {
        public:
            // Finds a path from (startX, startY) to (endX, endY) on the navmesh
            // Returns a vector of waypoints (Vector2)
            static std::vector<Vector2> pathfind(float startX, float startY,
                                                 float endX, float endY,
                                                 const NavMesh &navMesh);
        private:
            // Helper methods
            static ConvexPolygon* findContainingRegion(float x, float y, const NavMesh& navMesh);
            static std::optional<Vector2> findClosestPoint(float x, float y, const NavMesh& navMesh, ConvexPolygon*& region);
            static float heuristic(const Vector2& a, const Vector2& b);

            static bool pointInConvexPolygon(const Vector2& point, const ConvexPolygon& polygon);

            static float triangleArea(const Vector2& a, const Vector2& b, const Vector2& c);
            static std::optional<std::pair<Vector2, Vector2>> findSharedEdge(const ConvexPolygon& a, const ConvexPolygon& b);
            static std::vector<ConvexPolygon*> reconstructPath(const std::unordered_map<ConvexPolygon*, ConvexPolygon*>& cameFrom, ConvexPolygon* current);

            static std::vector<std::pair<Vector2, Vector2>> extractPortals(const std::vector<ConvexPolygon *> &regionPath);

            static std::vector<Vector2> applyFunnelAlgorithm(
        const std::vector<std::pair<Vector2, Vector2>>& portals,
        const Vector2& startPoint,
        const Vector2& endPoint);
        };

    } // collision
} // blipcade

#endif // PATHFINDING_H