//
// Created by Pavlo Yevsehnieiev
//

#include "palette685.h"

namespace blipcade::graphics {
    // Constructor
    Palette685::Palette685() {
        colors.reserve(256);

        // Generate the main 240 colors
        for (uint8_t r = 0; r < 6; ++r) {
            for (uint8_t g = 0; g < 8; ++g) {
                for (uint8_t b = 0; b < 5; ++b) {
                    Color color(
                        r * 51, // 0, 51, 102, 153, 204, 255
                        g * 36, // 0, 36, 72, 108, 144, 180, 216, 252
                        b * 63, // 0, 63, 126, 189, 252
                        255
                    );
                    colors.push_back(color);
                }
            }
        }

        // Generate 16 additional grays
        for (uint8_t i = 1; i < 16; ++i) {
            const uint8_t value = i * 17; // 17, 34, ..., 238, 255
            Color gray(value, value, value, 255);
            colors.push_back(gray);
        }

        // Add transparent color
        colors.emplace_back(0, 0, 0, 0); // Transparent

        // Ensure we have exactly 256 colors
        while (colors.size() < 256) {
            colors.emplace_back(0, 0, 0, 255); // Fill with black if needed
        }
    }

    // Destructor
    Palette685::~Palette685() = default;

    // color_to_index method
    uint8_t Palette685::color_to_index(uint8_t r, uint8_t g, uint8_t b) {
        // Clamp r, g, b to their respective ranges
        r = std::min(r, static_cast<uint8_t>(5));
        g = std::min(g, static_cast<uint8_t>(7));
        b = std::min(b, static_cast<uint8_t>(4));

        return static_cast<uint8_t>((40 * static_cast<uint16_t>(r)) + (5 * static_cast<uint16_t>(g)) + static_cast<
                                        uint16_t>(b));
    }

    uint8_t Palette685::find_closest_color_685(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        if (a == 0) {
            return 0xFF; // Transparent
        }

        const uint8_t r_val = static_cast<uint8_t>(std::round(r / 51.0));
        const uint8_t g_val = static_cast<uint8_t>(std::round(g / 36.0));
        const uint8_t b_val = static_cast<uint8_t>(std::round(b / 63.0));

        const uint8_t ro = std::min(r_val, static_cast<uint8_t>(5));
        const uint8_t go= std::min(g_val, static_cast<uint8_t>(7));
        const uint8_t bo = std::min(b_val, static_cast<uint8_t>(4));

        return color_to_index(ro, go, bo);
    }


    uint8_t Palette685::get_size() {
        return 255; // Represents 256 colors (0 to 255)
    }

    std::vector<Color> Palette685::get_colors() const {
        return colors; // Return a copy
    }

    std::array<Color, 256> Palette685::getColorLookup() const {
        std::array<Color, 256> colorLookup;
        for (size_t i = 0; i < colors.size(); ++i) {
            colorLookup[i] = colors[i];
        }
        return colorLookup;
    }

    Color Palette685::get_color(uint8_t index) const {
        if (index >= colors.size()) {
            // Handle out-of-bounds access
            return {};
        }
        return colors[index];
    }

    Color Palette685::get_virtual_color(uint8_t index) const {
        if (index >= virtual_palette.colors.size()) {
            return {};
        }
        uint8_t virtual_index = virtual_palette.colors[index];
        if (virtual_index >= colors.size()) {
            return {};
        }
        return colors[virtual_index];
    }

    uint8_t Palette685::get_virtual_color_index(uint8_t index) const {
        if (index >= virtual_palette.colors.size()) {
            return 0;
        }
        return virtual_palette.colors[index];
    }

    void Palette685::swap_colors(uint8_t index1, uint8_t index2) {
        if (index1 >= colors.size() || index2 >= colors.size()) {
            return;
        }
        std::swap(colors[index1], colors[index2]);
    }

    void Palette685::swap_colors_virtual(uint8_t index1, uint8_t index2) {
        if (index1 >= virtual_palette.colors.size() || index2 >= virtual_palette.colors.size()) {
            return;
        }
        std::swap(virtual_palette.colors[index1], virtual_palette.colors[index2]);
    }

    void Palette685::reset_colors() {
        virtual_palette.colors.clear();
        virtual_palette.colors.reserve(256);
        for (uint16_t i = 0; i <= 255; ++i) {
            virtual_palette.colors.push_back(static_cast<uint8_t>(i));
        }
    }

    void Palette685::change_colors_virtual(const uint8_t index1, const uint8_t index2) {
        if (index1 >= virtual_palette.colors.size()) {
            return;
        }
        virtual_palette.colors[index1] = index2;
    }
} // graphics
// blipcade
