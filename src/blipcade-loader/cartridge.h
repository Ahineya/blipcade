//
// Created by Pavlo Yevsehnieiev
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <string>
#include <vector>
#include <collider.h>
#include <navmesh.h>

namespace blipcade::graphics {
    class Spritesheet;
}

namespace blipcade {
    class Cartridge {
    public:
        Cartridge(std::string code, std::unordered_map<std::string, graphics::Spritesheet> spritesheets,
              std::unordered_map<std::string, collision::Collider> colliders, std::unordered_map<std::string, collision::NavMesh> navmeshes);

        // Delete copy constructor and copy assignment to prevent accidental copying
        Cartridge(const Cartridge&) = delete;
        Cartridge& operator=(const Cartridge&) = delete;

        // Default move constructor and move assignment
        Cartridge(Cartridge&&) = default;
        Cartridge& operator=(Cartridge&&) = default;

        ~Cartridge();

        static Cartridge fromJson(const std::string &cartJson);

        [[nodiscard]] std::string getCode() const {
            return code;
        }

        [[nodiscard]] const std::unordered_map<std::string, graphics::Spritesheet> &getSpritesheets();

        [[nodiscard]] const std::unordered_map<std::string, collision::Collider> &getColliders();

        [[nodiscard]] const std::unordered_map<std::string, collision::NavMesh> &getNavmeshes();

    private:
        std::string code;
        std::unordered_map<std::string, graphics::Spritesheet> spritesheets;
        std::unordered_map<std::string, collision::Collider> colliders;
        std::unordered_map<std::string, collision::NavMesh> navmeshes;
    };
} // blipcade

#endif //CARTRIDGE_H
