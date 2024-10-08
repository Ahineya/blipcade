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

    std::vector<std::vector<Point> > preparePolygon(const std::vector<Vector2> &vertices) {
        std::vector<Vector2> cleanedVertices = vertices;

        // Remove duplicate last vertex if it matches the first one
        if (!cleanedVertices.empty() &&
            cleanedVertices.front().x == cleanedVertices.back().x &&
            cleanedVertices.front().y == cleanedVertices.back().y) {
            cleanedVertices.pop_back();
        }

        std::vector<std::vector<Point> > polygon;

        // Log how many vertices are in the polygon
        std::cout << "PREPARE: VERTICES SIZE: " << cleanedVertices.size() << std::endl;

        // Create the outer ring
        std::vector<Point> outerRing;
        outerRing.reserve(cleanedVertices.size());

        for (const auto &vertex: cleanedVertices) {
            outerRing.emplace_back(Point{vertex.x, vertex.y});
        }

        polygon.emplace_back(std::move(outerRing));

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

            // Log how many vertices are in the polygon
            std::cout << "VERTICES SIZE: " << this->vertices.size() << std::endl;

            // Check winding order
            float signedArea = computeSignedArea();
            if (signedArea > 0.0f) {
                // Positive area indicates CCW
                std::cout << "POLYGON IS CCW" << std::endl;
                std::reverse(this->vertices.begin(), this->vertices.end());
            }

            // Here we want to build navmesh from the polygon

            std::cout << "PREPARE POLYGON" << std::endl;
            const auto polygon = preparePolygon(this->vertices);
            std::cout << "PREPARE POLYGON DONE" << std::endl;

            // log how many points are in the polygon
            std::cout << "POLYGON SIZE: " << polygon.size() << std::endl;
            // log polygon
            for (const auto &point: polygon) {
                std::cout << "POINT: " << point[0][0] << ", " << point[0][1] << std::endl;
            }

            std::vector<N> indices = mapbox::earcut<N>(polygon);

            std::cout << "INDICES SIZE: " << indices.size() << std::endl;

            // Extract triangles
            triangles.reserve(indices.size() / 3);
            for (std::size_t i = 0; i < indices.size(); i += 3) {
                Triangle tri = Triangle(Vector2{0, 0}, Vector2{0, 0}, Vector2{0, 0});
                tri.v1 = this->vertices[indices[i]];
                tri.v2 = this->vertices[indices[i + 1]];
                tri.v3 = this->vertices[indices[i + 2]];
                triangles.push_back(tri);
            }

            std::cout << "TRIANGLES SIZE: " << triangles.size() << std::endl;

            // Print all triangles. They have a friend std::ostream &operator<<(std::ostream &os, const Triangle &triangle) function
            for (const auto &tri: triangles) {
                std::cout << tri << std::endl;
            }

            // Reverse the vertices back to ensure CCW winding order
            std::reverse(this->vertices.begin(), this->vertices.end());
        }
    }

    Collider Collider::fromJson(const nlohmann::json &colliderJson) {
        auto const strType = colliderJson["type"].get<std::string>();
        auto const type = colliderTypeFromString(strType);

        std::vector<Vector2> vertices;
        for (const auto &vertexJson: colliderJson["vertices"]) {
            vertices.push_back({vertexJson["x"].get<float>(), vertexJson["y"].get<float>()});
        }

        return {type, vertices};
    }

    float Collider::computeSignedArea() const {
        float area = 0.0f;
        size_t n = vertices.size();
        for (size_t i = 0; i < n; ++i) {
            const Vector2 &current = vertices[i];
            const Vector2 &next = vertices[(i + 1) % n];
            area += (current.x * next.y) - (next.x * current.y);
        }
        return 0.5f * area;
    }

    bool Collider::checkCollisionPoint(const Vector2 &point) const {
        switch (type) {
            case ColliderType::RECTANGLE:
                return CheckCollisionPointRec(point, Rectangle{
                                                  vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y
                                              });
            case ColliderType::CIRCLE:
                return CheckCollisionPointCircle(point, Vector2{vertices[0].x, vertices[0].y}, vertices[1].x);
            case ColliderType::CONVEX_POLYGON:
            case ColliderType::CONCAVE_POLYGON:
                return CheckCollisionPointPoly(point, vertices.data(), vertices.size());
            case ColliderType::POINT:
                return CheckCollisionPointRec(point, Rectangle{vertices[0].x, vertices[0].y, 1, 1});
            case ColliderType::LINE:
                return CheckCollisionPointLine(point, Vector2{vertices[0].x, vertices[0].y},
                                               Vector2{vertices[1].x, vertices[1].y}, 1);
            case ColliderType::RAY:
                // return CheckCollisionPointRay(point, Vector2{vertices[0].x, vertices[0].y},
                // Vector2{vertices[1].x, vertices[1].y});
                return false;
            default:
                return false;
        }
    }
} // collision
// blipcade
