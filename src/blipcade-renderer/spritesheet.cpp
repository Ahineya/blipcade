//
// Created by Pavlo Yevsehnieiev
//

#include "spritesheet.h"

#include <iomanip>
#include <iostream>


namespace blipcade::graphics {
    Spritesheet::Spritesheet(const uint32_t width, const uint32_t height): pixelBuffer(width * height),
                                                                           sprites(std::vector<Sprite>()),
                                                                           width(width),
                                                                           height(height) {
    }

    Spritesheet::~Spritesheet() {
        pixelBuffer.clear();
        sprites.clear();
    }

    Spritesheet Spritesheet::fromData(const std::vector<uint8_t>& pixelBuffer, const std::vector<uint32_t>& spriteData,
                                      uint32_t width, uint32_t height) {
        auto spritesheet = Spritesheet(width, height);
        spritesheet.pixelBuffer = pixelBuffer;

        for (size_t i = 0; i < spriteData.size() / 5; i++) {
            const uint32_t x = spriteData[i * 5];
            const uint32_t y = spriteData[i * 5 + 1];
            const uint32_t sprWidth = spriteData[i * 5 + 2];
            const uint32_t sprHeight = spriteData[i * 5 + 3];
            const uint32_t flags = spriteData[i * 5 + 4];

            std::cout << "Adding sprite: x: " << x << ", y: " << y << ", width: " << sprWidth << ", height: " << sprHeight << ", flags: " << std::bitset<8>(flags) << "\n";
            spritesheet.sprites.push_back(Sprite{x, y, sprWidth, sprHeight, flags});
        }

        spritesheet.createTexture();

        return spritesheet;
    }

    void Spritesheet::createTexture() {
        Image image;
        image.data = pixelBuffer.data();
        image.width = static_cast<int>(width);
        image.height = static_cast<int>(height);
        image.mipmaps = 1;
        image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE; // Single-channel format

        texture = LoadTextureFromImage(image);

        std::cout << "Texture created: width:" << texture.width << ", height: " << texture.height << "\n";

        // Set texture filtering to point sampling to avoid color bleeding
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    }

    void Spritesheet::addSprite(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t flags) {
        std::cout << "Adding sprite: x: " << x << ", y: " << y << ", width: " << width << ", height: " << height << ", flags: " << std::bitset<8>(flags) << "\n";
        sprites.push_back(Sprite{x, y, width, height, flags});
    }

    const Sprite &Spritesheet::getSprite(const uint32_t index) const {
        if (index < sprites.size()) {
            return sprites[index];
        }

        std::cerr << "Invalid sprite index: " << index << "\n";
        return sprites[0];
    }

    std::vector<uint8_t> Spritesheet::getSpriteData(const size_t index) const {
        const auto &[x, y, width, height, flags] = getSprite(index);
        return getRectangleData(x, y, width, height);
    }

    std::vector<uint8_t> Spritesheet::getRectangleData(const uint32_t x0, const uint32_t y0, const uint32_t width,
                                                       const uint32_t height) const {
        if ((x0 + width) > this->width || (y0 + height) > this->height) {
            std::cerr << "Invalid rectangle: (" << x0 << ", " << y0 << ") - (" << width << ", " << height << ")\n";
            std::cerr << "Spritesheet size: (" << this->width << ", " << this->height << ")\n";
            return {};
        }

        std::vector<uint8_t> data;
        data.reserve(width * height);

        for (uint32_t y = y0; y < y0 + height; y++) {
            const auto start = y * this->width + x0;
            const auto end = start + width;

            if (end <= pixelBuffer.size()) {
                data.insert(data.end(), pixelBuffer.begin() + start, pixelBuffer.begin() + end);
            } else {
                return data;
            }
        }

        return data;
    }

    std::ostream &operator<<(std::ostream &os, const Spritesheet &spritesheet) {
        os << std::dec<< "Spritesheet {\n"
                << "  width: " << spritesheet.width << ",\n"
                << "  height: " << spritesheet.height << ",\n"
                << "  pixelBuffer size: " << spritesheet.pixelBuffer.size() << ",\n"
                << "  sprites: [\n";

        for (const auto &sprite: spritesheet.sprites) {
            os << std::dec<< "    Sprite { "
                    << "x: " << std::setw(3) << sprite.x << ", "
                    << "y: " << std::setw(3) << sprite.y << ", "
                    << "width: " << std::setw(3) << sprite.width << ", "
                    << "height: " << std::setw(3) << sprite.height << ", "
                    << "flags: 0b" << std::bitset<8>(sprite.flags) << " },\n";
        }

        os << "  ]\n}";

        return os;
    }
} // graphics
// blipcade
