// include/virtual_palette.h
#ifndef VIRTUAL_PALETTE_H
#define VIRTUAL_PALETTE_H

#include <vector>
#include <cstdint>

struct VirtualPalette {
    std::vector<uint8_t> colors;

    VirtualPalette() {
        colors.reserve(256);
        for (uint16_t i = 0; i <= 255; ++i) {
            colors.push_back(static_cast<uint8_t>(i));
        }
    }

    // Clone method
    VirtualPalette clone() const {
        VirtualPalette vp;
        vp.colors = this->colors;
        return vp;
    }
};

#endif // VIRTUAL_PALETTE_H