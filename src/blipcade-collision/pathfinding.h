// pathfinding.h

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "navmesh.h"
#include <vector>
#include <optional>
#include <raymath.h>

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

        struct PathPoint {
            Vector2 *point;
            ConvexPolygon *region;
            PathPoint *parent;
            float gCost; // Cost from start
            float hCost; // Heuristic cost to end
            float fCost() const { return gCost + hCost; }

            PathPoint(ConvexPolygon* r, Vector2* pp, float g, float h, PathPoint* p = nullptr)
                : region(r), point(pp), gCost(g), hCost(h), parent(p) {}

            // Copy constructor to handle deep copy of pointer members
            PathPoint(const PathPoint &other)
                : region(other.region), point(new Vector2(*other.point)), gCost(other.gCost),
                  hCost(other.hCost), parent(other.parent ? new PathPoint(*other.parent) : nullptr) {}

            ~PathPoint() {
            }

            bool operator==(const PathPoint &other) const {
                return Vector2Equals(*point, *other.point) && region == other.region;
            }
        };

        // Comparator for the priority queue
        struct ComparePathPoint {
            bool operator()(const PathPoint& a, const PathPoint& b) const {
                return a.fCost() > b.fCost();
            }
        };

        struct PathPointHash {
            std::size_t operator()(const PathPoint& p) const {
                std::hash<float> floatHasher;
                std::size_t h1 = floatHasher(p.point->x);
                std::size_t h2 = floatHasher(p.point->y);
                std::size_t h3 = std::hash<ConvexPolygon*>()(p.region);
                return h1 ^ (h2 << 1) ^ (h3 << 2);
            }
        };

        class Pathfinding {
        public:
            // Finds a path from (startX, startY) to (endX, endY) on the navmesh
            // Returns a vector of waypoints (Vector2)
            static std::vector<Vector2> pathfind(float startX, float startY,
                                                 float endX, float endY,
                                                 const NavMesh &navMesh);
            static std::vector<Vector2> pathfind(float startX, float startY, float endX, float endY, const NavMesh &navMesh,
                                          bool custom);

            static std::vector<Vector2> cleanPath(const std::vector<Vector2> &path, const std::vector<std::pair<Vector2, Vector2>> &meshOutline);

            static std::vector<Vector2> reconstructPath(PathPoint *current);

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