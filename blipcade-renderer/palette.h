//
// Created by Pavlo Yevsehnieiev on 26.09.2024.
//

#ifndef PALETTE_H
#define PALETTE_H

#include "color.h"
#include <vector>
#include <cstdint>
#include <any>

namespace blipcade {
    namespace graphics {



    class Palette {
    public:
        virtual ~Palette() = default;

        // Pure virtual methods corresponding to the Rust trait
        virtual std::any as_any() const = 0;
        virtual uint8_t get_size() const = 0;
        virtual std::vector<Color> get_colors() const = 0;
        virtual Color get_color(uint8_t index) const = 0;
        virtual Color get_virtual_color(uint8_t index) const = 0;
        virtual uint8_t get_virtual_color_index(uint8_t index) const = 0;
        virtual void swap_colors(uint8_t index1, uint8_t index2) = 0;
        virtual void swap_colors_virtual(uint8_t index1, uint8_t index2) = 0;
        virtual void reset_colors() = 0;
        virtual void change_colors_virtual(uint8_t index1, uint8_t index2) = 0;

        // Clone method
        virtual Palette* clone() const = 0;
    };


} // graphics
} // blipcade

#endif //PALETTE_H
