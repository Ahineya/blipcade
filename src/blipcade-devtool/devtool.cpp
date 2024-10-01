//
// Created by Pavlo Yevsehnieiev
//

#include "devtool.h"

#include <imgui.h>
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

    using PropertyPair = std::pair<std::string, quickjs::value>;

    std::vector<PropertyPair> getObjectProperties(quickjs::context &ctx, const quickjs::value &object) {
        std::vector<PropertyPair> properties;

        // Get the global Object constructor
        quickjs::value Object = ctx.get_global_object().get_property("Object");

        // Call Object.keys on the object to retrieve property names
        quickjs::value keys = Object.call_member("keys", object);

        if (!keys.is_array()) {
            // If keys is not an array, return empty properties
            return properties;
        }

        uint32_t length = keys.get_property("length").as_uint32();

        for (uint32_t i = 0; i < length; ++i) {
            quickjs::value key = keys.get_property(i);
            if (key.is_string()) {
                std::string keyStr = key.as_cstring().c_str();
                quickjs::value value = object.get_property(keyStr.c_str());
                properties.emplace_back(keyStr, value);
            }
        }

        return properties;
    }

    // Recursive function to draw the properties of the component
    void Devtool::RenderECSInspector() const {
        auto const ecs = runtime.getECS();
        auto const ctx = runtime.getContext();
        auto entities = ecs->getActiveEntities();
        ImGui::Text("Entities: %d", static_cast<int>(entities.size()));
        ImGui::Separator();

        for (auto entity: entities) {
            ImGui::PushID(entity); // Assuming 'entity' is unique

            auto const tag = ecs->getComponent(entity, "Tag");

            if (!tag.is_undefined()) {
                ImGui::Text("Entity: %s", tag.as_cstring().c_str());
            } else {
                ImGui::Text("Entity: %d", entity);
            }

            ImGui::Indent();

            if (ImGui::TreeNode("Components")) {
                std::unordered_map<ecs::ComponentTypeID, quickjs::value> components = ecs->getComponents(entity);
                for (auto [typeID, component]: components) {
                    // Push a unique ID for each component within the entity scope
                    ImGui::PushID(typeID); // Assuming 'typeID' is unique within the entity

                    auto const componentName = ecs->getComponentName(typeID);
                    ImGui::Text("Component: %s", componentName.c_str());

                    ImGui::Indent();

                    // Retrieve properties using Object.keys
                    std::vector<PropertyPair> properties = getObjectProperties(*ctx, component);

                    if (!component.is_object()) {
                        ImGui::Text("%s", component.as_cstring().c_str());
                        ImGui::Unindent();
                        ImGui::PopID(); // Pop component ID
                        continue;
                    }

                    for (const auto &[name, value]: properties) {
                        if (value.is_object()) {
                            if (ImGui::TreeNode(name.c_str())) {
                                // No need to push ID here if 'name' is unique within component
                                drawObjectRecursive(*ctx, name, value);
                                ImGui::TreePop();
                            }
                        } else {
                            ImGui::Text("%s: %s", name.c_str(), value.as_cstring().c_str());
                        }
                    }

                    ImGui::Unindent();
                    ImGui::PopID(); // Pop component ID
                }

                ImGui::TreePop();
            }

            ImGui::Unindent();
            ImGui::Separator();
            ImGui::PopID(); // Pop entity ID
        }
    }

    void Devtool::drawObjectRecursive(quickjs::context& ctx, const std::string& prefix, const quickjs::value& object) const {
        // Push a unique ID based on the prefix
        ImGui::PushID(prefix.c_str());

        std::vector<PropertyPair> properties = getObjectProperties(ctx, object);

        for (const auto& [name, value] : properties) {
            std::string displayName = prefix.empty() ? name : prefix + "." + name;

            if (value.is_object()) {
                if (!value.is_null() && !value.is_undefined()) {
                    if (ImGui::TreeNode(name.c_str())) {
                        // Recursively push IDs based on the current property name
                        drawObjectRecursive(ctx, displayName, value);
                        ImGui::TreePop();
                    }
                } else {
                    ImGui::Text("%s: null/undefined", displayName.c_str());
                }
            } else {
                ImGui::Text("%s: %s", displayName.c_str(), value.as_cstring().c_str());
            }
        }

        ImGui::PopID();
    }
}
