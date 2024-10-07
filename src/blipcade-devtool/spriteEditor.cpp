//
// Created by Pavlo Yevsehnieiev
//

#include "spriteEditor.h"

#include <devtool.h>
#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <palette685.h>
#include <raylib.h>
#include <rlImGui.h>

namespace blipcade::devtool {
    static Texture2D texture;

    SpriteEditor::SpriteEditor(Devtool &devtool) : devtool(devtool), active(false) {
    }

    void SpriteEditor::SetActive(bool active) {
        this->active = active;
    }

    bool SpriteEditor::IsActive() const {
        return active;
    }

    void SpriteEditor::Draw() {
        if (!active) return;



        ImGui::Begin("Sprites Editor", &active, ImGuiWindowFlags_MenuBar);

        const auto windowSize = ImGui::GetWindowSize();

        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Open sprite")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png", config);
            }

            // Let's create submenus for the copying active polygon or all polygons
            // if (ImGui::BeginMenu("Copy")) {
            //     if (ImGui::MenuItem("Copy Active Polygon") && selectedPolygon != -1) {
            //         CopyPolygonData(polygons[currentEditingPolygon]);
            //     }
            //     if (ImGui::MenuItem("Copy All Polygons")) {
            //         CopyAllPolygonData();
            //     }
            //     ImGui::EndMenu();
            // }

            // if (ImGui::MenuItem("Close")) {
            // active = false;
            // }
            ImGui::EndMenuBar();
        }

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action
                texture = LoadTexture(filePathName.c_str());
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        if (texture.id == 0) {
            ImGui::Text("No spritesheet loaded");
            ImGui::End();
            return;
        }


        // Make two columns
        ImGui::Columns(2, "SpriteSheetColumns", false);
        // First column
        // get current window width

        auto columnWidth = std::clamp(ImGui::GetWindowWidth() / 2, 0.0f, 400.0f);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::SetColumnWidth(1, ImGui::GetWindowWidth() - columnWidth);
        ImGui::BeginChild("SpriteSheet", ImVec2(0, 0), true);
        ImGui::Text("SpriteSheet");
        rlImGuiImage(&texture);
        ImGui::EndChild();

        // Second column
        ImGui::NextColumn();
        ImGui::BeginChild("SpriteSheetInfo", ImVec2(0, 0), true);
        ImGui::Text("SpriteSheet Info");
        ImGui::Text("Width: %d", texture.width);
        ImGui::Text("Height: %d", texture.height);
        ImGui::Text("Mipmaps: %d", texture.mipmaps);
        ImGui::Text("Format: %d", texture.format);

        if (ImGui::Button("Copy to clipboard")) {
            saveTexture();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    void SpriteEditor::saveTexture() {
        // Here we want to get an image of the texture, transform it into blipcart format and copy it to the clipboard

        // First we need to get the image data with Raylib
        graphics::Palette685 palette;

        Image image = LoadImageFromTexture(texture);
        // image.data // This is the image data in RGBA format
        // image.width // The width of the image
        // image.height // The height of the image

        // So we now can go pixel by pixel and transform it into blipcart format

        std::vector<uint8_t> blipcartData;
        blipcartData.reserve(image.width * image.height);

        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                Color pixel = GetImageColor(image, x, y);

                const auto index = palette.find_closest_color_685(pixel.r, pixel.g, pixel.b, pixel.a);
                blipcartData.push_back(index);
            }
        }

        // log the data to the console as a hex string
        std::string hexString;
        for (auto &pixel : blipcartData) {
            hexString += std::format("{:02X}", pixel);
            hexString += " ";
        }

        // Remove the last space
        hexString.pop_back();

        std::cout << hexString << std::endl;
        ImGui::SetClipboardText(hexString.c_str());
    }
} // devtool
// blipcade
