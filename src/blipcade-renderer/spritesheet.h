//
// Created by Pavlo Yevsehnieiev
//

#ifndef SPRITESHEET_H
#define SPRITESHEET_H
#include <cstdint>
#include <raylib.h>
#include <unordered_map>
#include <vector>

namespace blipcade::graphics {
    struct Sprite {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
        uint32_t flags;
    };

    class Spritesheet {
    public:
        Spritesheet(uint32_t width, uint32_t height);
        ~Spritesheet();

        static Spritesheet fromData(const std::vector<uint8_t> &pixelBuffer, const std::vector<uint32_t> &spriteData, uint32_t width, uint32_t height);
        static Spritesheet fromResource(const std::string &resourcePath, const std::string &projectDir);

        void addSprite(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t flags);

        const Sprite &getSprite(uint32_t index) const;

        [[nodiscard]] std::vector<uint8_t> getSpriteData(size_t index) const;
        [[nodiscard]] std::vector<uint8_t> getRectangleData(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) const;

        std::vector<uint8_t> pixelBuffer{};
        std::vector<Sprite> sprites;

        // std::unordered_map<std::string,

        friend std::ostream& operator<<(std::ostream& os, const Spritesheet& spritesheet);

        uint32_t width;
        uint32_t height;

        Texture2D texture;
        void createTexture();

        std::vector<Color> colorData;
    private:


    };
} // graphics
// blipcade

#endif //SPRITESHEET_H
