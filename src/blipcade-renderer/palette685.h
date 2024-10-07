// include/palette685.h
#ifndef PALETTE685_H
#define PALETTE685_H

#include "palette.h"
#include "color.h"
#include "virtual_palette.h"
#include <vector>

namespace blipcade::graphics {
    class Palette685 final {
    public:
        // Constructor
        Palette685();

        // Destructor
        ~Palette685();

        std::vector<Color> colors;
        VirtualPalette virtual_palette;

        // Methods
        static uint8_t color_to_index(uint8_t r, uint8_t g, uint8_t b);

        static uint8_t find_closest_color_685(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

        // to_image will be defined later

        // Palette interface implementations

        [[nodiscard]] static uint8_t get_size() ;

        [[nodiscard]] std::vector<Color> get_colors() const;
        [[nodiscard]] std::array<Color, 256> getColorLookup() const;

        [[nodiscard]] Color get_color(uint8_t index) const;

        [[nodiscard]] Color get_virtual_color(uint8_t index) const;

        [[nodiscard]] uint8_t get_virtual_color_index(uint8_t index) const;

        void swap_colors(uint8_t index1, uint8_t index2);

        void swap_colors_virtual(uint8_t index1, uint8_t index2);

        void reset_colors();

        void change_colors_virtual(uint8_t index1, uint8_t index2);
    };
}
#endif // PALETTE685_H
