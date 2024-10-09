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
#include <fstream>
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
            // std::cout << "VERTICES SIZE: " << this->vertices.size() << std::endl;

            // Check winding order
            float signedArea = computeSignedArea();
            if (signedArea > 0.0f) {
                // Positive area indicates CCW
                // std::cout << "POLYGON IS CCW" << std::endl;
                std::reverse(this->vertices.begin(), this->vertices.end());
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

    Collider Collider::fromResource(const std::string &resourcePath, const std::string &projectDir) {
        std::cout << "Loading spritesheet from resource: " << resourcePath << "\n";

        const auto path = resourcePath.substr(6);

        std::filesystem::path fullPath = std::filesystem::path(projectDir) / path;
        nlohmann::json colliderJson;

        std::ifstream colliderStream(fullPath.lexically_normal());
        colliderStream >> colliderJson;

        return fromJson(colliderJson);
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
