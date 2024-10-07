// src/devtool/PolygonEditor.h
#pragma once

#include "polygon.h"
#include "runtime.h"
// #include "utils/Clipboard.h"
#include <vector>
#include <raylib.h>
#include "navmesh.h"
#include <nlohmann/json_fwd.hpp>

namespace blipcade::devtool {
    class Devtool;
}

namespace blipcade::devtool {
    class PolygonEditor {
    public:
        PolygonEditor(Devtool& devtool);
        ~PolygonEditor();

        void Draw(); // Draw the ImGui window for polygon management
        void RenderPolygons(); // Render polygons over the game
        void RenderNavMeshes();

        void HandleInput(); // Handle mouse and keyboard input for polygon creation
        void CopyPolygonData(const Polygon& polygon); // Copy polygon data to clipboard and console
        void CopyAllPolygonData();

        [[nodiscard]] nlohmann::json PolygonsToJson() const; // Convert polygons to JSON
        nlohmann::json PolygonToJson(const Polygon &polygon) const;

        // Toggle visibility
        void SetActive(bool active);
        bool IsActive() const;

        void RemovePolygon(int index);

    private:
        Devtool& devtool;
        bool active;

        std::vector<Polygon> polygons;

        std::unordered_map<size_t, collision::NavMesh> navmeshes;

        int currentEditingPolygon; // -1 indicates no polygon being edited
        int selectedPolygon; // -1 indicates no selection

        // Helper Methods
        [[nodiscard]] Vector2 GameToScreen(const Vector2& gamePos) const;
        [[nodiscard]] Vector2 ScreenToGame(const Vector2& screenPos) const;

        void GenerateNavmesh(Polygon &polygon);

        // UI State
        bool show_demo_window;
    };
}
