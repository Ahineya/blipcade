//
// Created by Pavlo Yevsehnieiev
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <string>
#include <vector>

namespace blipcade::graphics {
    class Spritesheet;
}

namespace blipcade {
    class Cartridge {
    public:
        Cartridge(
            std::string code,
            const std::vector<graphics::Spritesheet>& spritesheets
            );

        ~Cartridge();

        static Cartridge fromJson(const std::string &cartJson);

        [[nodiscard]] std::string getCode() const {
            return code;
        }

        [[nodiscard]] const std::vector<graphics::Spritesheet> &getSpritesheets();

        [[nodiscard]] graphics::Spritesheet &getSpritesheet(uint32_t index);

    private:
        std::string code;
        std::vector<graphics::Spritesheet> spritesheets;
    };
} // blipcade

#endif //CARTRIDGE_H
