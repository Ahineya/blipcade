//
// Created by Pavlo Yevsehnieiev
//

#include "devtool.h"

#include <imgui.h>
#include <runtime.h>
#include <algorithm> // For std::transform

#include "imgui-style.h"

#include <ImGuiFileDialog.h>

namespace blipcade::devtool {
    Devtool::Devtool(runtime::Runtime &runtime)
        : active(false), runtime(runtime), tagFilter(""), polygonEditor(*this), spriteEditor(*this) {
        // Initialize the buffer with the empty string
        std::strncpy(tagFilterBuffer, tagFilter.c_str(), sizeof(tagFilterBuffer));
        // Ensure null-termination
        tagFilterBuffer[sizeof(tagFilterBuffer) - 1] = '\0';
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

    void Devtool::setScale(float scale) {
        this->scale = scale;
    }

    void Devtool::setCanvasOffset(const Vector2 &offset) {
        this->canvasOffset = offset;
    }

    void Devtool::setCanvasSize(const Vector2 &size) {
        this->canvasSize = size;
    }

    float Devtool::getScale() const {
        return scale;
    }

    Vector2 Devtool::getCanvasOffset() const {
        return canvasOffset;
    }

    Vector2 Devtool::getCanvasSize() const {
        return canvasSize;
    }

    static bool show_demo_window = false;

    void Devtool::init() {
        // embraceTheDarkness();
        ImGui::SetupImGuiStyle(true, 0.9);

    }

    void Devtool::draw() {
        // We want the window to be in the top left corner
        // Also we want it to fit the content.
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Blipcade Devtool", &active, ImGuiWindowFlags_MenuBar);
        ImGui::PushFont(ImGui::font1);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Sprite Editor", "")) { spriteEditor.SetActive(true); }
            if (ImGui::MenuItem("Polygon Editor", "")) { polygonEditor.SetActive(true); }
            if (ImGui::MenuItem("Close", "Ctrl+W")) { active = false; }
            ImGui::EndMenuBar();
        }





        ImGui::Text("FPS: %f", FPS);

        // Add a separator and the filter input
        ImGui::Separator();
        ImGui::Text("Filter Entities by Tag:");

        // Display the InputText widget with the buffer
        if (ImGui::InputText("##TagFilter", tagFilterBuffer, sizeof(tagFilterBuffer))) {
            // Update the std::string when the buffer changes
            tagFilter = std::string(tagFilterBuffer);
        }
        ImGui::Separator();

        // Draw the ECS inspector
        RenderECSInspector();

        if (polygonEditor.IsActive()) {
            polygonEditor.Draw();
        }

        if (spriteEditor.IsActive()) {
            spriteEditor.Draw();
        }

        ImGui::PopFont();
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

void Devtool::RenderECSInspector() const {
    auto const ecs = runtime.getECS();
    auto const ctx = runtime.getContext();
    auto entities = ecs->getActiveEntities();

    ImGui::Text("Entities: %d", static_cast<int>(entities.size()));
    ImGui::Separator();

    for (auto entity: entities) {
        // Retrieve the Tag component
        auto const tag = ecs->getComponent(entity, "Tag");

        // Check if the entity has a Tag component
        if (!tag.is_undefined()) {
            std::string entityTag = tag.as_cstring().c_str();

            // If a filter is set, check if the entity's tag contains the filter string
            if (!tagFilter.empty()) {
                // Case-insensitive search
                std::string lowerEntityTag = entityTag;
                std::string lowerFilter = tagFilter;
                std::transform(lowerEntityTag.begin(), lowerEntityTag.end(), lowerEntityTag.begin(), ::tolower);
                std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);

                if (lowerEntityTag.find(lowerFilter) == std::string::npos) {
                    continue; // Skip entities that don't match the filter
                }
            }

            ImGui::PushID(entity); // Assuming 'entity' is unique
            ImGui::Text("Entity: %s", entityTag.c_str());
        } else {
            // If no Tag component, handle based on whether a filter is active
            if (!tagFilter.empty()) {
                continue; // Skip entities without a Tag if a filter is applied
            }

            ImGui::PushID(entity); // Assuming 'entity' is unique
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
                            drawObjectRecursive(*ctx, name, value);
                            ImGui::TreePop();
                        }
                    } else if (value.is_number()) {
                        auto v = static_cast<float>(value.as_double());

                        // We want DragFloat to have max width of 100
                        ImGui::PushID((name + std::to_string(entity)).c_str());
                        ImGui::Text("%s:", name.c_str());
                        ImGui::SameLine();
                        ImGui::PushItemWidth(100);
                        ImGui::DragFloat("##xx", &v);
                        ImGui::PopItemWidth();
                        ImGui::PopID();
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
