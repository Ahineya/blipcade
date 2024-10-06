// pathfinding.cpp

#include "pathfinding.h"

#include <raymath.h>
#include <stdexcept>
#include <vector>
#include <optional>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>

namespace blipcade::collision {
    // Helper function to reconstruct the path using the cameFrom map
    std::vector<ConvexPolygon *> Pathfinding::reconstructPath(
        const std::unordered_map<ConvexPolygon *, ConvexPolygon *> &cameFrom,
        ConvexPolygon *current) {

        std::vector<ConvexPolygon *> path;
        while (current != nullptr) {
            path.push_back(current);
            auto it = cameFrom.find(current);
            if (it != cameFrom.end()) {
                current = it->second;
            } else {
                current = nullptr;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    // pathfinding.cpp

    // TODO: It contains a memory leak, as the PathPoint objects are not deleted after the path is found.
    // Need to rewrite it to use smart pointers. Later.
    std::vector<Vector2> Pathfinding::pathfind(float startX, float startY,
                                               float endX, float endY,
                                               const NavMesh &navMesh, bool custom) {
        ConvexPolygon *startRegion = findContainingRegion(startX, startY, navMesh);
        ConvexPolygon *endRegion = findContainingRegion(endX, endY, navMesh);

        auto startPoint = Vector2{startX, startY};
        auto endPoint = Vector2{endX, endY};

        // If start or end points are not inside any region, find the closest point on the NavMesh
        if (!startRegion) {
            if (auto closestStart = findClosestPoint(startX, startY, navMesh, startRegion)) {
                startPoint = closestStart.value();
            } else {
                throw std::runtime_error("Start point is not on the NavMesh and no closest point found.");
            }
        }

        if (!endRegion) {
            if (auto closestEnd = findClosestPoint(endX, endY, navMesh, endRegion)) {
                endPoint = closestEnd.value();
            } else {
                throw std::runtime_error("End point is not on the NavMesh and no closest point found.");
            }
        }

        if (!startRegion || !endRegion) {
            throw std::runtime_error("Unable to determine start or end region.");
        }

        // If the start and end regions are the same, return a direct path
        if (startRegion == endRegion) {
            return {startPoint, endPoint};
        }

        std::priority_queue<PathPoint, std::vector<PathPoint>, ComparePathPoint> openList;
        std::unordered_set<PathPoint, PathPointHash> closedList;

        PathPoint startNavPoint(startRegion, &startPoint, 0.0f, heuristic(startPoint, endPoint), nullptr);
        closedList.insert(startNavPoint);

        PathPoint navPoint(startRegion, &startRegion->centroid, 0.0f, heuristic(startPoint, endPoint), &startNavPoint);
        openList.push(navPoint);

        std::unordered_map<Vector2*, Vector2*> cameFrom;
        std::unordered_set<ConvexPolygon*> visited;

        while (!openList.empty()) {
            auto current = openList.top();
            openList.pop();

            std::cout << "Current Point: (" << current.point->x << ", " << current.point->y << ")\n";
            std::cout << "Current Region: (" << current.region->centroid.x << ", " << current.region->centroid.y << ")\n";
            std::cout << "Size:" << openList.size() << std::endl;

            closedList.insert(current);

            auto currentRegion = current.region;
            if (visited.find(currentRegion) != visited.end()) {
                continue;
            }

            visited.insert(currentRegion);

            // Log current and end regions
            std::cout << "Current Region: (" << currentRegion->centroid.x << ", " << currentRegion->centroid.y << ")\n";
            std::cout << "End Region: (" << endRegion->centroid.x << ", " << endRegion->centroid.y << ")\n";

            if (currentRegion == endRegion) {
                std::cout << "We are in the end region" << std::endl;
                auto path = reconstructPath(&current);

                std::reverse(path.begin(), path.end());
                // Replace last point with the end point
                path[path.size() - 1] = endPoint;

                std::vector<std::pair<Vector2, Vector2>> lines;
                lines.reserve(path.size() - 1);
                for (size_t i = 0; i < path.size() - 1; ++i) {
                    lines.emplace_back(path[i], path[i + 1]);
                }

                const auto meshOutline = navMesh.getOutline(); // const std::vector<std::pair<Vector2, Vector2>>*
                auto const cleanedPath = cleanPath(path, *meshOutline);

                return cleanedPath;
            }

            std::cout << "Neighbors: " << currentRegion->neighbors.size() << std::endl;
            for (auto& neighbor : currentRegion->neighbors) {
                // Add centroid
                PathPoint navPoint(neighbor, &neighbor->centroid, current.gCost + Vector2Distance(*current.point, neighbor->centroid),
                                   heuristic(neighbor->centroid, endPoint), &current);

                bool found = false;
                for (auto& closedPoint : closedList) {
                    if (Vector2Equals(*closedPoint.point, neighbor->centroid)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    std::cout << "Adding point to open list: (" << neighbor->centroid.x << ", " << neighbor->centroid.y << ")\n";
                    std::cout << "With parent" << current.point->x << ", " << current.point->y << std::endl;
                    openList.push(PathPoint(navPoint));
                }
            }
        }

        return {};

        throw std::runtime_error("Not implemented.");
    }

    std::vector<Vector2> Pathfinding::cleanPath(const std::vector<Vector2> &path, const std::vector<std::pair<Vector2, Vector2>> &meshOutline) {
        std::vector<Vector2> pathCopy = {};
        for (auto point : path) {
            pathCopy.push_back(point);
        }

        // for each three points A, B, C in the path we want to construct a line AC, and check if it intersects with any of the lines in the mesh outline
        // if it does not, we remove B from the path
        // if it does, we keep B in the path
        // we repeat this process until we have a clean path, where we can't remove any more points

        bool changed;

        do {
            changed = false;
            for (size_t i = 0; i < pathCopy.size() - 2; ++i) {
                const Vector2& a = pathCopy[i];
                const Vector2& b = pathCopy[i + 1];
                const Vector2& c = pathCopy[i + 2];

                bool canRemoveB = true;
                for (const auto& outline : meshOutline) {
                    Vector2 intersection;
                    if (
                        CheckCollisionLines(a, c, outline.first, outline.second, &intersection)
                        && !Vector2Equals(a, outline.first) && !Vector2Equals(c, outline.second)
                        && !Vector2Equals(a, outline.second) && !Vector2Equals(c, outline.first)
                        && !Vector2Equals(intersection, a) && !Vector2Equals(intersection, c)
                        && !Vector2Equals(intersection, b)
                        ) {
                        canRemoveB = false;
                        break;
                    }
                }

                if (canRemoveB) {
                    pathCopy.erase(pathCopy.begin() + i + 1);
                    changed = true;
                    break;  // Restart the loop as we modified the path
                }
            }
        } while (changed);

        return pathCopy;
    }

    std::vector<Vector2> Pathfinding::reconstructPath(const PathPoint* current) {
        std::vector<Vector2> path;
        while (current != nullptr) {
            path.push_back(*current->point);
            current = current->parent;
        }
        return path;
    }

    ConvexPolygon *Pathfinding::findContainingRegion(const float x, const float y, const NavMesh &navMesh) {
        const auto point = Vector2{x, y};
        for (const auto &region: navMesh.regions) {
            if (pointInConvexPolygon(point, *region)) {
                return region.get();
            }
        }
        return nullptr;
    }

    bool Pathfinding::pointInConvexPolygon(const Vector2 &point, const ConvexPolygon &polygon) {
        // Using the ray-casting algorithm
        bool inside = false;
        const size_t n = polygon.vertices.size();
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Vector2 &vi = polygon.vertices[i];
            const Vector2 &vj = polygon.vertices[j];
            if (((vi.y > point.y) != (vj.y > point.y)) &&
                (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y + 0.00001f) + vi.x)) {
                inside = !inside;
            }
        }
        return inside;
    }

    std::optional<Vector2> Pathfinding::findClosestPoint(const float x, const float y, const NavMesh &navMesh,
                                                         ConvexPolygon *&regionOut) {
        Vector2 point = Vector2{x, y};
        float minDistSq = std::numeric_limits<float>::max();
        Vector2 closestPoint;
        ConvexPolygon *closestRegion = nullptr;

        for (const auto &region: navMesh.regions) {
            // Find the closest point on the polygon to the point
            for (size_t i = 0; i < region->vertices.size(); ++i) {
                size_t j = (i + 1) % region->vertices.size();
                Vector2 a = region->vertices[i];
                Vector2 b = region->vertices[j];

                // Project point onto the edge ab
                Vector2 ab = Vector2Subtract(b, a);
                Vector2 ap = Vector2Subtract(point, a);
                float abLenSq = Vector2DotProduct(ab, ab);
                float t = Vector2DotProduct(ap, ab) / (abLenSq + 0.00001f);
                t = Clamp(t, 0.0f, 1.0f);
                Vector2 projection = Vector2Add(a, Vector2Scale(ab, t));

                float distSq = Vector2DotProduct(Vector2Subtract(point, projection),
                                                 Vector2Subtract(point, projection));
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    closestPoint = projection;
                    closestRegion = region.get();
                }
            }
        }

        if (closestRegion) {
            regionOut = closestRegion;
            return closestPoint;
        }

        return std::nullopt;
    }

    float Pathfinding::heuristic(const Vector2 &a, const Vector2 &b) {
        return Vector2Distance(a, b); // Use Euclidean distance
    }
} // blipcade::collision
