// src/devtool/PolygonEditor.h
#pragma once

#include "polygon.h"
#include "runtime.h"
// #include "utils/Clipboard.h"
#include <vector>
#include <string>
#include <raylib.h>
#include <imgui.h>

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
        void HandleInput(); // Handle mouse and keyboard input for polygon creation
        void CopyPolygonData(const Polygon& polygon); // Copy polygon data to clipboard and console

        // Toggle visibility
        void SetActive(bool active);
        bool IsActive() const;

    private:
        Devtool& devtool;
        bool active;

        std::vector<Polygon> polygons;
        int currentEditingPolygon; // -1 indicates no polygon being edited
        int selectedPolygon; // -1 indicates no selection

        // Helper Methods
        Vector2 GameToScreen(const Vector2& gamePos) const;
        Vector2 ScreenToGame(const Vector2& screenPos) const;

        // UI State
        bool show_demo_window;
    };
}
