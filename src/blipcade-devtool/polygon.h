// src/devtool/Polygon.h
#pragma once

#include <vector>
#include <string>
#include <raylib.h>

namespace blipcade::devtool {
struct Polygon {
    std::vector<Vector2> vertices;
    std::string name; // Optional: to identify polygons
    bool isClosed = false; // Indicates if the polygon is closed
};
}