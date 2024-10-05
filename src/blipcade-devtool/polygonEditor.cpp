// src/devtool/PolygonEditor.cpp
#include "polygonEditor.h"

#include <devtool.h>
#include <imgui.h>
#include <raylib.h>
#include <iostream>
#include <navmesh.h>
#include <sstream>
#include <raymath.h>
#include <triangulation.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace blipcade::devtool {
    PolygonEditor::PolygonEditor(Devtool &devtool)
        : devtool(devtool), active(false),
          polygons(), navmeshes(),
          currentEditingPolygon(-1), selectedPolygon(-1), show_demo_window(false) {
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
                if (ImGui::MenuItem("Copy Active Polygon") && selectedPolygon != -1) {
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
        // Add button to generate navmesh if the polygon is selected
        if (selectedPolygon != -1) {
            if (ImGui::Button("Generate Navmesh")) {
                // Generate navmesh
                GenerateNavmesh(polygons[selectedPolygon]);
            }
        }
        ImGui::EndGroup();

        ImGui::End();

        HandleInput();

        RenderNavMeshes();
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
                    selectedPolygon = currentEditingPolygon;
                    GenerateNavmesh(polygons[selectedPolygon]);
                    currentEditingPolygon = -1; // Finish editing
                    // select the polygon
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
            selectedPolygon = currentEditingPolygon;
            GenerateNavmesh(polygons[selectedPolygon]);
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
                }

                // For polygon navigation mesh, we will draw the filled polygon with PathFillConvex:
                const auto navmesh = navmeshes.find(i);
                if (navmesh != navmeshes.end()) {
                    for (const auto &region : navmesh->second.regions) {
                        ImVec2 *points = new ImVec2[region->vertices.size()];
                        std::transform(region->vertices.begin(), region->vertices.end(), points, [this](const Vector2 &v) {
                            auto const coords = GameToScreen(v);
                            return ImVec2(coords.x, coords.y);
                        });

                        drawList->AddConvexPolyFilled(points, static_cast<int>(region->vertices.size()), fillColor);
                    }
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

    void PolygonEditor::RenderNavMeshes() {
        // for each navmesh
        for (const auto &navMesh : navmeshes) {
            for (size_t i = 0; i < navMesh.second.regions.size(); ++i) {
                for (size_t j = 0; j < navMesh.second.regions[i]->vertices.size(); ++j) {
                    const Vector2 &v1 = navMesh.second.regions[i]->vertices[j];
                    const Vector2 &v2 = navMesh.second.regions[i]->vertices[(j + 1) % navMesh.second.regions[i]->vertices.size()];

                    const auto coords1 = GameToScreen(v1);
                    const auto coords2 = GameToScreen(v2);

                    ImGui::GetBackgroundDrawList()->AddLine(
                        ImVec2(coords1.x, coords1.y),
                        ImVec2(coords2.x, coords2.y),
                        IM_COL32(0, 0, 255, 255), 2.0f
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

        // Output to console navmeshes
        for (const auto &navMesh : navmeshes) {
            std::cout << "NavMesh:\n";
            std::cout << navMesh.second.toJson().dump() << std::endl;
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

        // round to nearest integer
        x = std::round(x);
        y = std::round(y);

        return Vector2{x, y};
    }

    void PolygonEditor::GenerateNavmesh(Polygon &polygon) {

        // 1. Exclude the last vertex if the polygon is closed
        std::vector<Vector2> vertices = polygon.vertices;
        if (polygon.isClosed) {
            vertices.pop_back();
        }

        std::cout << "Generating navmesh for polygon with " << vertices.size() << " vertices" << std::endl;

        const auto indices = collision::Triangulation::triangulate(vertices);

        collision::NavMesh navMesh;


        // Partition polygon into convex regions
        // auto convexRegions = partitionIntoConvex(this->vertices);
        std::vector<std::vector<Vector2>> convexRegions;

        // Let's partition the polygon into convex regions
        std::vector<Vector2> region;
        for (size_t i = 0; i < indices.size(); ++i) {
            region.push_back(vertices[indices[i]]);
            if (i % 3 == 2) {
                convexRegions.push_back(region);
                region.clear();
            }
        }

        std::cout << "Partitioned into " << convexRegions.size() << " convex regions.\n";

        // Add convex regions to navMesh
        for (auto& region : convexRegions) {
            navMesh.addRegion(region);
        }

        navMesh.buildConnectivity();

        navmeshes.emplace(selectedPolygon, std::move(navMesh));

    }
}
