//
// Created by Pavlo Yevsehnieiev
//

#include "devtool.h"

#include <imgui.h>
#include <math.h>
#include <runtime.h>

namespace blipcade::devtool {
    Devtool::Devtool(runtime::Runtime &runtime): active(false), runtime(runtime) {
    }

    Devtool::~Devtool() {
    }

    bool Devtool::isActive() {
        return active;
    }

    bool Devtool::setActive(bool active) {
        this->active = active;
    }

    void Devtool::setFPS(double FPS) {
        this->FPS = FPS;
    }

    static bool show_demo_window = false;

    void Devtool::draw() {


        // We want the window to be in the top left corner
        // Also we want it to fit the content.
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Blipcade Devtool", &active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Show demo UI", "")) { show_demo_window = true; }
        }
        if (ImGui::MenuItem("Close", "Ctrl+W")) { active = false; }
        ImGui::EndMenuBar();

        ImGui::Text("FPS: %f", FPS);

        // Draw the ECS inspector
        auto ecs = runtime.getECS();
        RenderECSInspector();

        ImGui::End();

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }

    // Recursive function to draw the properties of the component
    void Devtool::RenderECSInspector() const {
        auto const ecs = runtime.getECS();
        auto entities = ecs->getActiveEntities();
        ImGui::Text("Entities: %d", entities.size());
        for (auto entity : entities) {
            ImGui::Text("Entity: %d", entity);
            std::unordered_map<ecs::ComponentTypeID, quickjs::value> components = ecs->getComponents(entity);
            for (auto [typeID, component] : components) {

                auto const componentName = ecs->getComponentName(typeID);
                ImGui::Text("Component: %s", componentName.c_str());

                auto properties = ecs->getComponentProperties(component);
                for (auto [name, value] : properties) { // need to do this recursively, because value can be an object
                    ImGui::Text("%s: %s", name.c_str(), value.as_cstring().c_str());
                }
            }
        }
    }
}
