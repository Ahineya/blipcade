//
// Created by Pavlo Yevsehnieiev
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <string>
#include <vector>
#include <collider.h>

namespace blipcade::graphics {
    class Spritesheet;
}

namespace blipcade {
    class Cartridge {
    public:
        Cartridge(std::string code, const std::vector<graphics::Spritesheet> &spritesheets,
                  const std::vector<collision::Collider> &colliders);

        ~Cartridge();

        static Cartridge fromJson(const std::string &cartJson);

        [[nodiscard]] std::string getCode() const {
            return code;
        }

        [[nodiscard]] const std::vector<graphics::Spritesheet> &getSpritesheets();

        const std::vector<collision::Collider> &getColliders();

        [[nodiscard]] graphics::Spritesheet &getSpritesheet(uint32_t index);

    private:
        std::string code;
        std::vector<graphics::Spritesheet> spritesheets;
        std::vector<collision::Collider> colliders;
    };
} // blipcade

#endif //CARTRIDGE_H
