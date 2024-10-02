//
// Created by Pavlo Yevsehnieiev
//

// #include "collider.h"

#include <collider.h>
#include <collision.h>
#include <raylib.h>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <earcut.hpp>
#include <iostream>

namespace blipcade::collision {

    using Coord = double;
    using Point = std::array<Coord, 2>;
    using N = uint32_t;

    std::vector<std::vector<Point>> preparePolygon(const std::vector<Vector2>& vertices) {
        std::vector<Vector2> cleanedVertices = vertices;

        // Remove duplicate last vertex if it matches the first one
        if (!cleanedVertices.empty() &&
            cleanedVertices.front().x == cleanedVertices.back().x &&
            cleanedVertices.front().y == cleanedVertices.back().y) {
            cleanedVertices.pop_back();
            }


        std::vector<std::vector<Point>> polygon;

        for (const auto &vertex : cleanedVertices) {
            polygon.push_back({{vertex.x, vertex.y}});
        }

        return polygon;
    }

    Collider::Collider(ColliderType type, std::vector<Vector2> vertices): type(type), vertices(std::move(vertices)) {
        if (type == ColliderType::CONVEX_POLYGON || type == ColliderType::CONCAVE_POLYGON) {
            // Remove duplicate last vertex if present
            if (!this->vertices.empty() &&
                this->vertices.front().x == this->vertices.back().x &&
                this->vertices.front().y == this->vertices.back().y) {
                this->vertices.pop_back();
                }

            std::cout << "PREPARE POLYGON" << std::endl;
            const auto polygon = preparePolygon(vertices);
            std::cout << "PREPARE POLYGON DONE" << std::endl;


            std::vector<N> indices = mapbox::earcut<N>(polygon);

            // Extract triangles
            triangles.reserve(indices.size() / 3);
            for (std::size_t i = 0; i < indices.size(); i += 3) {
                Triangle tri = Triangle(Vector2{0, 0}, Vector2{0, 0}, Vector2{0, 0});
                tri.v1 = this->vertices[indices[i]];
                tri.v2 = this->vertices[indices[i + 1]];
                tri.v3 = this->vertices[indices[i + 2]];
                triangles.push_back(tri);
            }
        }
    }

    Collider Collider::fromJson(const nlohmann::json &colliderJson) {
        auto const strType = colliderJson["type"].get<std::string>();
        auto const type = colliderTypeFromString(strType);

        std::vector<Vector2> vertices;
        for (const auto &vertexJson : colliderJson["vertices"]) {
            vertices.push_back({vertexJson["x"].get<float>(), vertexJson["y"].get<float>()});
        }

        return {type, vertices};
    }
} // collision
// blipcade