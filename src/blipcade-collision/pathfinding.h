// pathfinding.h

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "navmesh.h"
#include <vector>
#include <optional>
#include <raymath.h>

namespace blipcade::collision {
    struct PathPoint {
        Vector2 *point;
        ConvexPolygon *region;
        PathPoint *parent;
        float gCost; // Cost from start
        float hCost; // Heuristic cost to end
        [[nodiscard]] float fCost() const { return gCost + hCost; }

        PathPoint(ConvexPolygon *r, Vector2 *pp, float g, float h, PathPoint *p = nullptr)
            : region(r), point(pp), gCost(g), hCost(h), parent(p) {
        }

        // Copy constructor to handle deep copy of pointer members
        PathPoint(const PathPoint &other)
            : region(other.region), point(new Vector2(*other.point)), gCost(other.gCost),
              hCost(other.hCost), parent(other.parent ? new PathPoint(*other.parent) : nullptr) {
        }

        ~PathPoint() = default;

        bool operator==(const PathPoint &other) const {
            return Vector2Equals(*point, *other.point) && region == other.region;
        }
    };

    // Comparator for the priority queue
    struct ComparePathPoint {
        bool operator()(const PathPoint &a, const PathPoint &b) const {
            return a.fCost() > b.fCost();
        }
    };

    struct PathPointHash {
        std::size_t operator()(const PathPoint &p) const {
            constexpr std::hash<float> floatHasher;
            const std::size_t h1 = floatHasher(p.point->x);
            const std::size_t h2 = floatHasher(p.point->y);
            const std::size_t h3 = std::hash<ConvexPolygon *>()(p.region);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    class Pathfinding {
    public:
        static std::vector<Vector2> pathfind(float startX, float startY, float endX, float endY, const NavMesh &navMesh,
                                             bool custom);

        static std::vector<Vector2> cleanPath(const std::vector<Vector2> &path,
                                              const std::vector<std::pair<Vector2, Vector2> > &meshOutline);

        static std::vector<Vector2> reconstructPath(const PathPoint *current);

    private:
        // Helper methods
        static ConvexPolygon *findContainingRegion(float x, float y, const NavMesh &navMesh);

        static std::optional<Vector2> findClosestPoint(float x, float y, const NavMesh &navMesh,
                                                       ConvexPolygon *&regionOut);

        static float heuristic(const Vector2 &a, const Vector2 &b);

        static bool pointInConvexPolygon(const Vector2 &point, const ConvexPolygon &polygon);

        static std::vector<ConvexPolygon *> reconstructPath(
            const std::unordered_map<ConvexPolygon *, ConvexPolygon *> &cameFrom, ConvexPolygon *current);
    };
} // collision
// blipcade

#endif // PATHFINDING_H
