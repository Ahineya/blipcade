//
// Created by Pavlo Yevsehnieiev
//

#include "canvas.h"

namespace blipcade::graphics {
    Canvas::Canvas(const uint32_t width, const uint32_t height): palette(std::make_unique<Palette685>()) {
        pixels.resize(width * height);

        std::ranges::fill(pixels, 0x00);

        pixels[64 * 128 + 64] = 0xfe;

        for (int i = 0; i < 256; i++) {
            virtualPalette[i] = static_cast<uint8_t>(i);
        }
    }

    Canvas::~Canvas() = default;

    const std::vector<uint8_t> &Canvas::getPixels() const {
        return pixels;
    }
}
