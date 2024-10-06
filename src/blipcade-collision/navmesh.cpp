//
// Created by Pavlo Yevsehnieiev
//

#include "navmesh.h"

#include <raymath.h>
#include <unordered_set>
#include <nlohmann/json.hpp>

namespace blipcade::collision {
    // Forward declaratio

    // Structure to represent a convex polygon region
    ConvexPolygon::ConvexPolygon(std::vector<Vector2> verts) : vertices(std::move(verts)) {
        // We want to ensure that the vertices are in CCW order
        // This is important for the centroid calculation
        // Let's compute the signed area
        float signedArea = 0.0f;
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Vector2 &current = vertices[i];
            const Vector2 &next = vertices[(i + 1) % vertices.size()];
            signedArea += (current.x * next.y) - (next.x * current.y);
        }

        // If the area is negative, reverse the vertices
        if (signedArea < 0.0f) {
            std::reverse(vertices.begin(), vertices.end());
        }
    }

    // Function to check if this polygon shares an edge with another
    bool ConvexPolygon::sharesEdge(const ConvexPolygon &other) const {
        int shared = 0;
        for (const auto &v1: vertices) {
            for (const auto &v2: other.vertices) {
                if (Vector2Equals(v1, v2)) shared++;
                if (shared >= 2) return true;
            }
        }
        return false;
    }

    // pathfinding.cpp

    bool vectorsAreEqual(const Vector2& a, const Vector2& b, float epsilon = 1e-5f) {
        return fabs(a.x - b.x) < epsilon && fabs(a.y - b.y) < epsilon;
    }

    std::optional<std::pair<Vector2, Vector2>> ConvexPolygon::getSharedEdge(const ConvexPolygon& other) const {
        std::vector<std::pair<Vector2, Vector2>> sharedEdges;

        for (size_t i = 0; i < vertices.size(); ++i) {
            const Vector2& this_v1 = vertices[i];
            const Vector2& this_v2 = vertices[(i + 1) % vertices.size()];

            for (size_t j = 0; j < other.vertices.size(); ++j) {
                const Vector2& other_v1 = other.vertices[j];
                const Vector2& other_v2 = other.vertices[(j + 1) % other.vertices.size()];

                if (vectorsAreEqual(this_v1, other_v1) && vectorsAreEqual(this_v2, other_v2)) {
                    sharedEdges.emplace_back(std::make_pair(this_v1, this_v2));
                }

                if (vectorsAreEqual(this_v1, other_v2) && vectorsAreEqual(this_v2, other_v1)) {
                    sharedEdges.emplace_back(std::make_pair(this_v2, this_v1)); // Reverse order
                }
            }
        }

        if (!sharedEdges.empty()) {
            // Assuming only one shared edge exists
            return sharedEdges[0];
        }

        return std::nullopt;
    }

    // Function to add a convex region
    void NavMesh::addRegion(std::vector<Vector2> verts) {
        regions.emplace_back(std::make_shared<ConvexPolygon>(std::move(verts)));
    }

    void NavMesh::calculateCentroids() {
        for (auto &region : regions) {
            region->calculateCentroid();
        }
    }

    // Function to establish connectivity between regions
    void NavMesh::buildConnectivity() {
        for (size_t i = 0; i < regions.size(); ++i) {
            for (size_t j = i + 1; j < regions.size(); ++j) {
                if (regions[i]->sharesEdge(*regions[j])) {
                    regions[i]->neighbors.push_back(regions[j].get());
                    regions[j]->neighbors.push_back(regions[i].get());
                }
            }
        }

        for (size_t i = 0; i < regions.size(); ++i) {
            std::cout << "Region " << i << " has " << regions[i]->neighbors.size() << " neighbors.\n";
        }
    }

    bool NavMesh::isPointOnNavMesh(Vector2 point) const {
        for (const auto &region : regions) {
            if (pointInTriangle(point, *region) || Vector2Equals(point, region->vertices[0]) || Vector2Equals(point, region->vertices[1]) || Vector2Equals(point, region->vertices[2])) {
                return true;
            }
        }

        std::cout << "Point is not on the navmesh.\n";
        std::cout << "Point: " << point.x << ", " << point.y << "\n";
        std::cout << "Checks:\n";
        std::cout << "is point in triangle: " << pointInTriangle(point, *regions[0]) << "\n";
        std::cout << "is point equal to vertex 0: " << Vector2Equals(point, regions[0]->vertices[0]) << "\n";
        std::cout << "is point equal to vertex 1: " << Vector2Equals(point, regions[0]->vertices[1]) << "\n";
        std::cout << "is point equal to vertex 2: " << Vector2Equals(point, regions[0]->vertices[2]) << "\n";
        return false;
    }

    bool NavMesh::pointInTriangle(const Vector2 point, const ConvexPolygon triangle) const {
        // Barycentric coordinates
        float alpha = ((triangle.vertices[1].y - triangle.vertices[2].y) * (point.x - triangle.vertices[2].x) +
                       (triangle.vertices[2].x - triangle.vertices[1].x) * (point.y - triangle.vertices[2].y)) /
                      ((triangle.vertices[1].y - triangle.vertices[2].y) * (triangle.vertices[0].x - triangle.vertices[2].x) +
                       (triangle.vertices[2].x - triangle.vertices[1].x) * (triangle.vertices[0].y - triangle.vertices[2].y));
        float beta = ((triangle.vertices[2].y - triangle.vertices[0].y) * (point.x - triangle.vertices[2].x) +
                      (triangle.vertices[0].x - triangle.vertices[2].x) * (point.y - triangle.vertices[2].y)) /
                     ((triangle.vertices[1].y - triangle.vertices[2].y) * (triangle.vertices[0].x - triangle.vertices[2].x) +
                      (triangle.vertices[2].x - triangle.vertices[1].x) * (triangle.vertices[0].y - triangle.vertices[2].y));
        float gamma = 1.0f - alpha - beta;

        return alpha > 0 && beta > 0 && gamma > 0;
    }

    bool NavMesh::isLineCompletelyOnNavMesh(const Vector2 A, const Vector2 B) const {
        if (!isPointOnNavMesh(A) || !isPointOnNavMesh(B)) {
            std::cout << "One of the points is not on the navmesh.\n";
            return false;  // If either endpoint is not on the mesh, return false
        }

        Vector2 currentPoint = A;
        while (true) {
            std::shared_ptr<ConvexPolygon> currentRegion = findRegionContainingPoint(currentPoint);
            if (!currentRegion) {
                return false;  // This shouldn't happen if isPointOnNavMesh is correct
            }

            Vector2 exitPoint = {};
            std::cout << "BEFORE FIND NEXT REGION\n";
            std::shared_ptr<ConvexPolygon> nextRegion = findNextRegionAlongLine(currentRegion, currentPoint, B, exitPoint);
            std::cout << "AFTER FIND NEXT REGION\n";

            if (!nextRegion) {
                std::cout << "No next region found.\n";
                // We've reached B or there's no next region
                return (exitPoint.x == B.x && exitPoint.y == B.y);
            }

            std::cout << "Exit point: " << exitPoint.x << ", " << exitPoint.y << "\n";

            currentPoint = exitPoint;

            std::cout << "Current point: " << currentPoint.x << ", " << currentPoint.y << "\n";
        }
    }

    std::shared_ptr<ConvexPolygon> NavMesh::findRegionContainingPoint(const Vector2 point) const {
        for (const auto& region : regions) {
            if (pointInTriangle(point, *region)) {
                return region;
            }
        }
        return nullptr;
    }

    std::shared_ptr<ConvexPolygon> NavMesh::findNextRegionAlongLine(const std::shared_ptr<ConvexPolygon> currentRegion,
                                                           const Vector2 start, const Vector2 end,
                                                           Vector2& exitPoint) const {
        for (size_t i = 0; i < 3; ++i) {
            Vector2 edgeStart = currentRegion->vertices[i];
            Vector2 edgeEnd = currentRegion->vertices[(i + 1) % 3];

            Vector2 intersection = {};
            if (CheckCollisionLines(start, end, edgeStart, edgeEnd, &intersection)) {
                // Find the neighboring region that shares this edge
                for (const auto& neighbor : currentRegion->neighbors) {
                    if (neighbor->sharesEdge(*currentRegion)) {
                        return std::shared_ptr<ConvexPolygon>(neighbor);
                    }
                }
            }

            exitPoint.x = intersection.x;
            exitPoint.y = intersection.y;
        }

        // If we're here, the line ends in this region
        exitPoint = end;
        return nullptr;
    }

    nlohmann::json NavMesh::toJson() const {
        nlohmann::json j;

        // Step 1: Create a mapping from ConvexPolygon pointers to their indices
        std::unordered_map<const ConvexPolygon *, size_t> polygonIndexMap;
        for (size_t i = 0; i < regions.size(); ++i) {
            polygonIndexMap[regions[i].get()] = i;
        }

        // Step 2: Serialize each region
        j["regions"] = nlohmann::json::array();
        for (size_t i = 0; i < regions.size(); ++i) {
            const auto &region = regions[i];
            nlohmann::json regionJson;

            // Serialize vertices
            regionJson["vertices"] = nlohmann::json::array();
            for (const auto &vertex: region->vertices) {
                regionJson["vertices"].push_back({vertex.x, vertex.y});
            }

            // Serialize neighbors as indices
            regionJson["neighbors"] = nlohmann::json::array();
            for (const auto &neighbor: region->neighbors) {
                auto it = polygonIndexMap.find(neighbor);
                if (it != polygonIndexMap.end()) {
                    regionJson["neighbors"].push_back(it->second);
                } else {
                    // Handle the case where the neighbor is not found (optional)
                    std::cerr << "Warning: Neighbor not found in the regions list.\n";
                }
            }

            // Add the serialized region to the regions array
            j["regions"].push_back(regionJson);
        }

        return j;
    }

    // Helper function to calculate signed area
    float signedArea(const ConvexPolygon& polygon) {
        float area = 0.0f;
        size_t n = polygon.vertices.size();
        for (size_t i = 0; i < n; ++i) {
            const Vector2& current = polygon.vertices[i];
            const Vector2& next = polygon.vertices[(i + 1) % n];
            area += (current.x * next.y) - (next.x * current.y);
        }
        return area * 0.5f;
    }

    // Function to enforce consistent winding order (CCW by default)
    void ensureConsistentWinding(NavMesh& navMesh, bool clockwise /*= false*/) {
        for (auto& region : navMesh.regions) {
            float area = signedArea(*region);
            bool isClockwise = area < 0.0f;
            if (isClockwise != clockwise) {
                std::reverse(region->vertices.begin(), region->vertices.end());
                // Optionally recompute centroid if your ConvexPolygon class stores it
                // region->computeCentroid();
            }
        }
    }



    std::vector<std::pair<Vector2, Vector2> > NavMesh::getOutline() const {
        std::unordered_set<std::pair<Vector2, Vector2>, VectorPairHash, VectorPairHash > sharedEdges;
        std::vector<std::pair<Vector2, Vector2> > outline;

        // Get all shared edges
        for (size_t i = 0; i < regions.size(); ++i) {
            auto current = regions[i];

            for (size_t j = i + 1; j < regions.size(); ++j) {
                auto next = regions[j];

                auto sharedEdgeOpt = current->getSharedEdge(*next);
                if (sharedEdgeOpt.has_value()) {
                    std::pair<Vector2, Vector2> sharedEdge = sharedEdgeOpt.value();
                    // Order the edge so that the first vertex is less than the second
                    if (sharedEdge.first.x > sharedEdge.second.x) {
                        std::swap(sharedEdge.first, sharedEdge.second);
                    }

                    sharedEdges.insert(sharedEdge);
                }
            }
        }

        for (const auto& region: regions ) {
            for (size_t i = 0; i < region->vertices.size(); ++i) {
                Vector2 start = region->vertices[i];
                Vector2 end = region->vertices[(i + 1) % region->vertices.size()];

                std::pair<Vector2, Vector2> edge = std::make_pair(start, end);
                // Order the edge so that the first vertex is less than the second
                if (edge.first.x > edge.second.x) {
                    std::swap(edge.first, edge.second);
                }

                if (!sharedEdges.contains(edge)) {
                    outline.push_back(edge);
                }
            }
        }

        return outline;
    }

    NavMesh NavMesh::fromJson(const nlohmann::json &j) {
        NavMesh navMesh;

        // Validate the presence of "regions" in the JSON
        if (!j.contains("regions") || !j["regions"].is_array()) {
            throw std::invalid_argument("Invalid JSON format: Missing or incorrect 'regions' array.");
        }

        // Temporary storage for neighbor indices
        std::vector<std::vector<size_t> > neighborIndices;

        // Step 1: Reconstruct Convex Polygons
        for (const auto &regionJson: j["regions"]) {
            // Validate region structure
            if (!regionJson.contains("vertices") || !regionJson["vertices"].is_array()) {
                throw std::invalid_argument("Invalid JSON format: Each region must contain a 'vertices' array.");
            }

            // Extract vertices
            std::vector<Vector2> vertices;
            for (const auto &vertexJson: regionJson["vertices"]) {
                if (!vertexJson.is_array() || vertexJson.size() != 2) {
                    throw std::invalid_argument("Invalid JSON format: Each vertex must be an array of two numbers.");
                }
                float x = vertexJson[0].get<float>();
                float y = vertexJson[1].get<float>();
                vertices.emplace_back(Vector2{x, y});
            }

            // Add the convex polygon to regions
            navMesh.addRegion(std::move(vertices));

            // Extract neighbor indices if present
            if (regionJson.contains("neighbors")) {
                if (!regionJson["neighbors"].is_array()) {
                    throw std::invalid_argument("Invalid JSON format: 'neighbors' must be an array.");
                }
                std::vector<size_t> neighbors;
                for (const auto &neighborIndexJson: regionJson["neighbors"]) {
                    if (!neighborIndexJson.is_number_unsigned()) {
                        throw std::invalid_argument("Invalid JSON format: Neighbor indices must be unsigned numbers.");
                    }
                    neighbors.push_back(neighborIndexJson.get<size_t>());
                }
                neighborIndices.emplace_back(std::move(neighbors));
            } else {
                // If no neighbors specified, add an empty list
                neighborIndices.emplace_back();
            }
        }


        // Step 2: Establish Neighbor Relationships
        for (size_t i = 0; i < navMesh.regions.size(); ++i) {
            for (const auto &neighborIdx: neighborIndices[i]) {
                if (neighborIdx >= navMesh.regions.size()) {
                    throw std::out_of_range("Invalid neighbor index found in JSON.");
                }

                ConvexPolygon *currentRegion = navMesh.regions[i].get();
                ConvexPolygon *neighborRegion = navMesh.regions[neighborIdx].get();

                // Add neighbor if not already present
                if (std::find(currentRegion->neighbors.begin(), currentRegion->neighbors.end(), neighborRegion) ==
                    currentRegion->neighbors.end()) {
                    currentRegion->neighbors.push_back(neighborRegion);
                }

                // Also add the reverse relationship
                if (std::find(neighborRegion->neighbors.begin(), neighborRegion->neighbors.end(), currentRegion) ==
                    neighborRegion->neighbors.end()) {
                    neighborRegion->neighbors.push_back(currentRegion);
                }
            }
        }

        // Optional: Validate connectivity by printing
        for (size_t i = 0; i < navMesh.regions.size(); ++i) {
            std::cout << "Region " << i << " has " << navMesh.regions[i]->neighbors.size() << " neighbors.\n";
        }


        navMesh.calculateCentroids();

        return navMesh;
    }
}
