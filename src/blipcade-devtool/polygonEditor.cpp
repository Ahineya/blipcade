// src/devtool/PolygonEditor.cpp
#include "polygonEditor.h"

#include <devtool.h>
#include <imgui.h>
#include <raylib.h>
#include <iostream>
#include <sstream>
#include <raymath.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace blipcade::devtool {
    PolygonEditor::PolygonEditor(Devtool &devtool)
        : devtool(devtool), active(false),
          polygons(), currentEditingPolygon(-1),
          selectedPolygon(-1), show_demo_window(false) {
    }

    PolygonEditor::~PolygonEditor() {
    }

    void PolygonEditor::SetActive(bool active) {
        this->active = active;
    }

    bool PolygonEditor::IsActive() const {
        return active;
    }

    void PolygonEditor::Draw() {
        if (!active) return;

        ImGui::Begin("Polygon Editor", &active, ImGuiWindowFlags_MenuBar);

        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("New Polygon")) {
                Polygon newPoly;
                polygons.emplace_back(newPoly);
                currentEditingPolygon = static_cast<int>(polygons.size()) - 1;
                selectedPolygon = -1; // Deselect others
            }

            // Let's create submenus for the copying active polygon or all polygons
            if (ImGui::BeginMenu("Copy")) {
                if (ImGui::MenuItem("Copy Active Polygon") && currentEditingPolygon != -1) {
                    CopyPolygonData(polygons[currentEditingPolygon]);
                }
                if (ImGui::MenuItem("Copy All Polygons")) {
                    CopyAllPolygonData();
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Close")) {
                active = false;
            }
            ImGui::EndMenuBar();
        }

        // Columns Layout: Left - Polygon List, Right - Instructions
        ImGui::Columns(2, nullptr, false);

        // Left Column: List of Polygons
        ImGui::BeginChild("PolygonList", ImVec2(200, 0), true);
        ImGui::Text("Polygons:");
        ImGui::Separator();
        for (size_t i = 0; i < polygons.size(); ++i) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Polygon %zu", i + 1);
            if (ImGui::Selectable(buf, selectedPolygon == static_cast<int>(i))) {
                selectedPolygon = static_cast<int>(i);
                currentEditingPolygon = -1; // Stop editing if selecting another polygon
            }
        }
        ImGui::EndChild();

        ImGui::NextColumn();

        // Right Column: Instructions
        ImGui::BeginGroup();
        ImGui::Text("Selected Polygon: %d", selectedPolygon + 1);
        ImGui::TextWrapped(
            "Left-click to add vertices.\nPress 'C' or click close to a starting point to close the polygon.");
        ImGui::EndGroup();

        ImGui::End();

        HandleInput();

        RenderPolygons();

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }

    void PolygonEditor::HandleInput() {
        if (currentEditingPolygon == -1) return; // No polygon is being edited

        Polygon &poly = polygons[currentEditingPolygon];

        // Check for mouse clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 gamePos = ScreenToGame(mousePos); // Convert to game coordinates

            // Check if the new point is near the first point to auto-close
            if (poly.vertices.size() >= 3) {
                Vector2 first = poly.vertices[0];
                float distance = Vector2Distance(gamePos, first);
                if (distance < 10.0f) {
                    // Threshold for closing
                    poly.vertices.push_back(poly.vertices[0]);
                    poly.isClosed = true;
                    currentEditingPolygon = -1; // Finish editing
                    return;
                }
            }

            // Add vertex to the current polygon
            poly.vertices.push_back(gamePos);
        }

        // Check for pressing 'C' to close the polygon
        if (IsKeyPressed(KEY_C) && poly.vertices.size() >= 3) {
            // Push the first vertex to the end to close the polygon
            poly.vertices.push_back(poly.vertices[0]);
            poly.isClosed = true;
            currentEditingPolygon = -1; // Finish editing
        }
    }

    void PolygonEditor::RenderPolygons() {
        ImDrawList *drawList = ImGui::GetBackgroundDrawList();
        // Ensure that polygons are rendered after the game elements so they appear as overlays.

        for (size_t i = 0; i < polygons.size(); ++i) {
            const Polygon &poly = polygons[i];
            if (poly.vertices.empty()) continue;

            ImU32 lineColor = (i == selectedPolygon) ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255);
            ImU32 fillColor = (i == selectedPolygon) ? IM_COL32(255, 0, 0, 15) : IM_COL32(0, 255, 0, 15);

            if (poly.isClosed && poly.vertices.size() >= 3) {
                for (size_t j = 0; j < poly.vertices.size() - 1; ++j) {
                    // Let's draw lines with ImGui:
                    auto const coords1 = GameToScreen(poly.vertices[j]);
                    auto const coords2 = GameToScreen(poly.vertices[j + 1]);
                    drawList->AddLine(
                        ImVec2(coords1.x, coords1.y),
                        ImVec2(coords2.x, coords2.y),
                        lineColor, 2.0f
                    );

                    // And let's draw the filled polygon with PathFillConcave:
                    ImVec2 *points = new ImVec2[poly.vertices.size()];
                    std::transform(poly.vertices.begin(), poly.vertices.end(), points, [this](const Vector2 &v) {
                        auto const coords = GameToScreen(v);
                        return ImVec2(coords.x, coords.y);
                    });

                    drawList->AddConcavePolyFilled(points, static_cast<int>(poly.vertices.size()), fillColor);
                }
            } else {
                for (size_t j = 0; j < poly.vertices.size() - 1; ++j) {
                    auto const coords1 = GameToScreen(poly.vertices[j]);
                    auto const coords2 = GameToScreen(poly.vertices[j + 1]);
                    drawList->AddLine(
                        ImVec2(coords1.x, coords1.y),
                        ImVec2(coords2.x, coords2.y),
                        lineColor, 2.0f
                    );
                }

                // Draw lines from last vertex to current mouse position if editing
                if (i == currentEditingPolygon) {
                    Vector2 mousePos = GetMousePosition();
                    auto const coords = GameToScreen(poly.vertices.back());

                    drawList->AddLine(
                        ImVec2(coords.x, coords.y),
                        ImVec2(mousePos.x, mousePos.y),
                        lineColor, 2.0f
                    );
                }
            }
        }
    }

    void PolygonEditor::CopyPolygonData(const Polygon &polygon) {
        nlohmann::json json = PolygonToJson(polygon);

        ImGui::SetClipboardText(json.dump().c_str());
        // Output to console
        std::cout << "Copied Polygon Data:\n" << json.dump() << std::endl;
    }

    void PolygonEditor::CopyAllPolygonData() {
        nlohmann::json json = PolygonsToJson();

        ImGui::SetClipboardText(json.dump().c_str());
        // Output to console
        std::cout << "Copied All Polygon Data:\n" << json.dump() << std::endl;
    }

    nlohmann::json PolygonEditor::PolygonsToJson() const {
        nlohmann::json json;

        for (const auto &poly: polygons) {
            nlohmann::json polyJson;
            for (auto vertice: poly.vertices) {
                nlohmann::json vertexJson;
                vertexJson["x"] = vertice.x;
                vertexJson["y"] = vertice.y;
                polyJson.push_back(vertexJson);
            }
            json.push_back(polyJson);
        }

        return json;
    }

    nlohmann::json PolygonEditor::PolygonToJson(const Polygon &polygon) const {
        nlohmann::json json;

        for (auto vertice: polygon.vertices) {
            nlohmann::json vertexJson;
            vertexJson["x"] = vertice.x;
            vertexJson["y"] = vertice.y;
            json.push_back(vertexJson);
        }

        return json;
    }

    Vector2 PolygonEditor::GameToScreen(const Vector2 &gamePos) const {
        float x = gamePos.x * devtool.getScale() + devtool.getCanvasOffset().x;
        float y = gamePos.y * devtool.getScale() + devtool.getCanvasOffset().y;
        return Vector2{x, y};
    }

    Vector2 PolygonEditor::ScreenToGame(const Vector2 &screenPos) const {
        float x = (screenPos.x - devtool.getCanvasOffset().x) / devtool.getScale();
        float y = (screenPos.y - devtool.getCanvasOffset().y) / devtool.getScale();
        return Vector2{x, y};
    }
}
