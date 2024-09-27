//
// Created by Pavlo Yevsehnieiev
//

#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <cstdint>
#include <vector>

namespace blipcade::graphics {
    struct Sprite {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
        uint8_t flags;
    };

    class Spritesheet {
    public:
        Spritesheet(uint32_t width, uint32_t height);
        ~Spritesheet();

        static Spritesheet fromData(const std::vector<uint8_t>& pixelBuffer, const std::vector<uint8_t>& spriteData, uint32_t width, uint32_t height);

        const Sprite &getSprite(uint32_t index) const;

        std::vector<uint8_t> getSpriteData(size_t index) const;

        [[nodiscard]] std::vector<uint8_t> getRectangleData(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) const;

        std::vector<uint8_t> pixelBuffer{};
        std::vector<Sprite> sprites;

        friend std::ostream& operator<<(std::ostream& os, const Spritesheet& spritesheet);

    private:
        uint32_t width;
        uint32_t height;
    };
} // graphics
// blipcade

#endif //SPRITESHEET_H
