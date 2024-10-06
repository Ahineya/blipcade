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

    std::vector<std::pair<Vector2, Vector2> > Pathfinding::extractPortals(
        const std::vector<ConvexPolygon *> &regionPath) {
        std::vector<std::pair<Vector2, Vector2> > portals;

        for (size_t i = 0; i < regionPath.size() - 1; ++i) {
            ConvexPolygon *current = regionPath[i];
            ConvexPolygon *next = regionPath[i + 1];

            auto sharedEdgeOpt = current->getSharedEdge(*next);
            if (sharedEdgeOpt.has_value()) {
                std::pair<Vector2, Vector2> sharedEdge = sharedEdgeOpt.value();

                if (Vector2Distance(sharedEdge.first, sharedEdge.second) < 0.00001f) {
                    throw std::runtime_error("Shared edge is too small.");
                }

                portals.emplace_back(sharedEdge);
                std::cout << "Portal " << i << ": (" << sharedEdge.first.x << ", " << sharedEdge.first.y << ") to ("
                        << sharedEdge.second.x << ", " << sharedEdge.second.y << ")\n";
            } else {
                throw std::runtime_error("No shared edge found between consecutive regions.");
            }
        }

        return portals;
    }

    // Function to apply the Funnel Algorithm to the sequence of portals
    std::vector<Vector2> Pathfinding::applyFunnelAlgorithm(
        const std::vector<std::pair<Vector2, Vector2> > &portals,
        const Vector2 &startPoint,
        const Vector2 &endPoint) {
        std::vector<Vector2> path;
        if (portals.empty()) {
            path.emplace_back(startPoint);
            path.emplace_back(endPoint);
            return path;
        }

        // path.emplace_back(startPoint);

        // Initialize funnel
        Vector2 apex = startPoint;
        Vector2 left = portals[0].first;
        Vector2 right = portals[0].second;
        int portalIndex = 1;
        int n = static_cast<int>(portals.size());

        // Left and Right legs of the funnel
        Vector2 leftLeg = left;
        Vector2 rightLeg = right;

        std::cout << "Initial Portal: Left(" << left.x << ", " << left.y << ") "
                << "Right(" << right.x << ", " << right.y << ")\n";

        const int MAX_ITERATIONS = 1000;
        int iterations = 0;

        for (; portalIndex < n && iterations < MAX_ITERATIONS; ++portalIndex, ++iterations) {
            std::pair<Vector2, Vector2> portal = portals[portalIndex];
            std::cout << "Processing Portal " << portalIndex << ": Left(" << portal.first.x << ", " << portal.first.y
                    << ") Right(" << portal.second.x << ", " << portal.second.y << ")\n";

            // Left side
            Vector2 newLeft = portal.first;
            float crossLeft = triangleArea(apex, leftLeg, newLeft);
            std::cout << "crossLeft: " << crossLeft << "\n";

            if (crossLeft >= 0.0f) {
                float crossNewRight = triangleArea(apex, rightLeg, newLeft);
                std::cout << "crossNewRight: " << crossNewRight << "\n";

                if (crossNewRight < 0.0f) {
                    // Append the previous rightLeg to the path
                    path.emplace_back(rightLeg);
                    std::cout << "Appending Right Leg: (" << rightLeg.x << ", " << rightLeg.y << ")\n";
                    apex = rightLeg;
                    leftLeg = apex;
                    rightLeg = apex;
                    portalIndex = portalIndex - 1; // Reprocess the current portal
                    continue;
                } else {
                    leftLeg = newLeft;
                }
            }

            // Right side
            Vector2 newRight = portal.second;
            float crossRight = triangleArea(apex, rightLeg, newRight);
            std::cout << "crossRight: " << crossRight << "\n";

            if (crossRight <= 0.0f) {
                // Adjusted for Y-down
                float crossNewLeft = triangleArea(apex, leftLeg, newRight);
                std::cout << "crossNewLeft: " << crossNewLeft << "\n";

                if (crossNewLeft > 0.0f) {
                    // Append the previous leftLeg to the path
                    path.emplace_back(leftLeg);
                    std::cout << "Appending Left Leg: (" << leftLeg.x << ", " << leftLeg.y << ")\n";
                    apex = leftLeg;
                    leftLeg = apex;
                    rightLeg = apex;
                    portalIndex = portalIndex - 1; // Reprocess the current portal
                    continue;
                } else {
                    rightLeg = newRight;
                }
            }
        }

        if (iterations >= MAX_ITERATIONS) {
            throw std::runtime_error("Exceeded maximum iterations in funnel algorithm.");
        }

        // Append the end point
        path.emplace_back(endPoint);
        std::cout << "End Point: (" << endPoint.x << ", " << endPoint.y << ")\n";

        // Log the final path
        std::cout << "Final Path:" << std::endl;
        for (const auto &point: path) {
            std::cout << "(" << point.x << ", " << point.y << ") ";
        }
        std::cout << std::endl;

        return path;
    }

    class NavPoint {
    public:
        Vector2 point;
        Vector2 parent;
    };

    std::vector<Vector2> Pathfinding::pathfind(float startX, float startY,
                                               float endX, float endY,
                                               const NavMesh &navMesh, bool custom) {
        ConvexPolygon *startRegion = findContainingRegion(startX, startY, navMesh);
        ConvexPolygon *endRegion = findContainingRegion(endX, endY, navMesh);

        Vector2 startPoint = Vector2{startX, startY};
        Vector2 endPoint = Vector2{endX, endY};

        std::vector<std::optional<PathPoint>> traversed; // Used for path reconstruction

        // If start or end points are not inside any region, find the closest point on the NavMesh
        if (!startRegion) {
            auto closestStart = findClosestPoint(startX, startY, navMesh, startRegion);
            if (closestStart) {
                startPoint = closestStart.value();
            } else {
                throw std::runtime_error("Start point is not on the NavMesh and no closest point found.");
            }
        }

        if (!endRegion) {
            auto closestEnd = findClosestPoint(endX, endY, navMesh, endRegion);
            if (closestEnd) {
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

        /*
         *Now that you know on which triangle your starting point is located you can add this point to the closed list
         *and send all the adjacent points to the open list (and keep a record of its parent point — in this case the starting point).
         *The adjacent points in this case are all the triangle vertices
         */

        // Let's create a closed list to store the points that have been evaluated
        std::priority_queue<PathPoint, std::vector<PathPoint>, ComparePathPoint> openList;
        // Let's create an open list to store the points that need to be evaluated
        // std::unordered_set<PathPoint*> closedList;
        std::unordered_set<PathPoint, PathPointHash> closedList;

        // Add the starting point to the closed list
        PathPoint startNavPoint(startRegion, &startPoint, 0.0f, heuristic(startPoint, endPoint), nullptr);
        closedList.insert(startNavPoint);

        // Add the starting point to the open list
        // openList.push(startNavPoint);

        // startRegion->vertices
        // We want to add all startRegion->vertices to the open list
        for (auto& vertex : startRegion->vertices) {
            PathPoint navPoint(startRegion, &vertex, 0.0f, heuristic(vertex, endPoint), &startNavPoint);
            openList.push(navPoint);
        }

        std::unordered_map<Vector2*, Vector2*> cameFrom;
        std::unordered_set<ConvexPolygon*> visited;

        while (!openList.empty()) {
            auto current = openList.top();
            openList.pop();

            traversed.push_back(current);

            std::cout << "Current Point: (" << current.point->x << ", " << current.point->y << ")\n";
            std::cout << "Current Region: (" << current.region->centroid.x << ", " << current.region->centroid.y << ")\n";
            std::cout << "Size:" << openList.size() << std::endl;

            // Here we want to evaluate the adjacent points.
            // 1. Get the current region
            // 2. Get the adjacent regions
            // 3. For each adjacent region, get the vertices
            // 4. For each vertex, create a new PathPoint and add it to the open list
            // 5. Add the current point to the closed list
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

                //TODO: Possible memory leak here
                for (auto& point : traversed) {
                    delete point->point;
                    delete point->parent;
                }

                // construct lines
                std::vector<std::pair<Vector2, Vector2>> lines;
                lines.reserve(path.size() - 1);
                for (size_t i = 0; i < path.size() - 1; ++i) {
                    lines.emplace_back(path[i], path[i + 1]);
                }

                const auto meshOutline = navMesh.getOutline();
                for (auto line: meshOutline) {
                    std::cout << "Outline: (" << line.first.x << ", " << line.first.y << ") to ("
                            << line.second.x << ", " << line.second.y << ")\n";
                }

                for (auto line: lines) {
                    for (auto outline: meshOutline) {
                        Vector2 intersection;
                        if (
                            CheckCollisionLines(outline.first, outline.second, line.first, line.second, &intersection)
                            && !Vector2Equals(outline.first, line.first) && !Vector2Equals(outline.second, line.second)
                            && !Vector2Equals(outline.first, line.second) && !Vector2Equals(outline.second, line.first)
                            ) {
                            std::cout << "Intersection: (" << intersection.x << ", " << intersection.y << ")\n";
                        }
                    }
                }

                auto const cleanedPath = cleanPath(path, meshOutline);

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
                // for (auto& vertex : neighbor->vertices) {
                //     PathPoint navPoint(neighbor, &vertex, current.gCost + Vector2Distance(*current.point, vertex),
                //                        heuristic(vertex, endPoint), &current);
                //
                //     std::cout << "Current Point Address: " << current.point << std::endl;
                //     std::cout << "Parent Point Address: " << current.parent->point << std::endl;
                //
                //     // Check if the point is already in the closed list
                //     // auto it = closedList.find()
                //     bool found = false;
                //     for (auto& closedPoint : closedList) {
                //         if (Vector2Equals(*closedPoint.point, vertex)) {
                //             found = true;
                //             break;
                //         }
                //     }
                //
                //     if (!found) {
                //         std::cout << "Adding point to open list: (" << vertex.x << ", " << vertex.y << ")\n";
                //         std::cout << "With parent" << current.point->x << ", " << current.point->y << std::endl;
                //         openList.push(PathPoint(navPoint));
                //     }
                // }
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

    std::vector<Vector2> Pathfinding::reconstructPath(PathPoint* current) {
        std::vector<Vector2> path;
        while (current != nullptr) {
            path.push_back(*current->point);
            current = current->parent;
        }
        return path;
    }

    std::vector<Vector2> Pathfinding::pathfind(float startX, float startY,
                                               float endX, float endY,
                                               const NavMesh &navMesh) {
        // Step 1: Find start and end regions
        ConvexPolygon *startRegion = findContainingRegion(startX, startY, navMesh);
        ConvexPolygon *endRegion = findContainingRegion(endX, endY, navMesh);

        Vector2 startPoint = Vector2{startX, startY};
        Vector2 endPoint = Vector2{endX, endY};

        // If start or end points are not inside any region, find the closest point on the NavMesh
        if (!startRegion) {
            auto closestStart = findClosestPoint(startX, startY, navMesh, startRegion);
            if (closestStart) {
                startPoint = closestStart.value();
            } else {
                throw std::runtime_error("Start point is not on the NavMesh and no closest point found.");
            }
        }

        if (!endRegion) {
            auto closestEnd = findClosestPoint(endX, endY, navMesh, endRegion);
            if (closestEnd) {
                endPoint = closestEnd.value();
            } else {
                throw std::runtime_error("End point is not on the NavMesh and no closest point found.");
            }
        }

        if (!startRegion || !endRegion) {
            throw std::runtime_error("Unable to determine start or end region.");
        }

        // Step 2: A* Search Initialization
        // Priority queue ordered by fCost
        std::priority_queue<PathNode, std::vector<PathNode>, ComparePathNode> openSet;
        // To keep track of evaluated regions
        std::unordered_set<ConvexPolygon *> closedSet;

        // Initialize start node
        float hCostStart = heuristic(startPoint, endPoint);
        PathNode startNode(startRegion, 0.0f, hCostStart);
        openSet.push(startNode);

        // To keep track of the best gCost for each region
        std::unordered_map<ConvexPolygon *, float> gCostMap;
        gCostMap[startRegion] = 0.0f;

        // To keep track of parent regions
        std::unordered_map<ConvexPolygon *, ConvexPolygon *> cameFrom;

        // Step 3: A* Search Loop
        while (!openSet.empty()) {
            PathNode current = openSet.top();
            openSet.pop();

            ConvexPolygon *currentRegion = current.region;

            // If the region is already evaluated, skip
            if (closedSet.find(currentRegion) != closedSet.end()) {
                continue;
            }

            // Mark the region as evaluated
            closedSet.insert(currentRegion);

            // Check if we've reached the end region
            if (currentRegion == endRegion) {
                // Reconstruct path
                // After reconstructing the path
                std::vector<ConvexPolygon *> regionPath = reconstructPath(cameFrom, endRegion);

                // Debug: Print the number of regions in the path
                std::cout << "Number of regions in path: " << regionPath.size() << std::endl;
                for (size_t i = 0; i < regionPath.size(); ++i) {
                    std::cout << "Region " << i << ": Centroid ("
                            << regionPath[i]->centroid.x << ", "
                            << regionPath[i]->centroid.y << ")\n";
                }

                // Step 4: Extract Portals
                std::vector<std::pair<Vector2, Vector2> > portals = extractPortals(regionPath);

                // Step 5: Apply Funnel Algorithm
                std::vector<Vector2> waypoints = applyFunnelAlgorithm(portals, startPoint, endPoint);
                // Push the start point to the front
                waypoints.insert(waypoints.begin(), startPoint);

                std::cout << "FOUND PATH!" << std::endl;

                return waypoints;
            }

            // Examine neighbors
            for (ConvexPolygon *neighbor: currentRegion->neighbors) {
                if (closedSet.find(neighbor) != closedSet.end()) {
                    continue; // Already evaluated
                }

                std::cout << "Neighbor: (" << neighbor->centroid.x << ", " << neighbor->centroid.y << ")\n";

                // Calculate tentative gCost
                float tentativeGCost = current.gCost + Vector2Distance(currentRegion->centroid, neighbor->centroid);

                // If this path to neighbor is better, record it
                if (gCostMap.find(neighbor) == gCostMap.end() || tentativeGCost < gCostMap[neighbor]) {
                    std::cout << "Tentative G Cost: " << tentativeGCost << std::endl;

                    gCostMap[neighbor] = tentativeGCost;
                    float hCost = heuristic(neighbor->centroid, endPoint);
                    PathNode neighborNode(neighbor, tentativeGCost, hCost);
                    openSet.push(neighborNode);
                    cameFrom[neighbor] = currentRegion;
                }
            }
        }

        // If no path is found
        throw std::runtime_error("No path found between the specified points.");
    }

    float Pathfinding::triangleArea(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
        return ((b.x - a.x) * (c.y - a.y)) - ((c.x - a.x) * (b.y - a.y));
    }

    ConvexPolygon *Pathfinding::findContainingRegion(float x, float y, const NavMesh &navMesh) {
        Vector2 point = Vector2{x, y};
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
        size_t n = polygon.vertices.size();
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

    std::optional<Vector2> Pathfinding::findClosestPoint(float x, float y, const NavMesh &navMesh,
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
