//
// Created by Pavlo Yevsehnieiev
//

#include "triangulation.h"

#include <earcut.hpp>
#include <iostream>
#include <ostream>

namespace blipcade {
namespace collision {

    std::vector<std::vector<Point> > Triangulation::preparePolygon(const std::vector<Vector2> &vertices) {
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

    std::vector<unsigned> Triangulation::triangulate(const std::vector<Vector2> &vertices) {
        float signedArea = computeSignedArea(vertices);
        // let's create a copy of the vertices
        std::vector<Vector2> verticesCopy = vertices;

        if (signedArea > 0.0f) { // Positive area indicates CCW
            std::cout << "POLYGON IS CCW, reversing" << std::endl;
            std::reverse(verticesCopy.begin(), verticesCopy.end());
        }

        std::cout << "PREPARE POLYGON" << std::endl;

        const auto polygon = preparePolygon(verticesCopy);

        auto indices = mapbox::earcut<N>(polygon);

        // Log how many indices are in the polygon
        std::cout << "INDICES SIZE: " << indices.size() << std::endl;

        return indices;
    }

    float Triangulation::computeSignedArea(const std::vector<Vector2> &vertices) {
        float area = 0.0f;
        size_t n = vertices.size();
        for (size_t i = 0; i < n; ++i) {
            const Vector2& current = vertices[i];
            const Vector2& next = vertices[(i + 1) % n];
            area += (current.x * next.y) - (next.x * current.y);
        }
        return 0.5f * area;
    }
} // collision
} // blipcade