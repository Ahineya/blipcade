//
// Created by Pavlo Yevsehnieiev
//

#include "navmesh.h"

#include <raymath.h>
#include <nlohmann/json.hpp>

namespace blipcade::collision {
    // Forward declaratio

    // Structure to represent a convex polygon region
    ConvexPolygon::ConvexPolygon(std::vector<Vector2> verts) : vertices(std::move(verts)) {
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

    // Function to add a convex region
    void NavMesh::addRegion(std::vector<Vector2> verts) {
        regions.emplace_back(std::make_unique<ConvexPolygon>(std::move(verts)));
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

        return navMesh;
    }
}
