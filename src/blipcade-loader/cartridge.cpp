//
// Created by Pavlo Yevsehnieiev
//

#include "cartridge.h"

#include <converters.h>
#include <iostream>
#include <spritesheet.h>
#include <nlohmann/json.hpp>

namespace blipcade {
    Cartridge::Cartridge(
        std::string code,
        const std::vector<graphics::Spritesheet>& spritesheets,
        const std::vector<collision::Collider>& colliders
    ): code(std::move(code)), spritesheets(spritesheets), colliders(colliders) {}
    Cartridge::~Cartridge() {
        spritesheets.clear();
    }

    Cartridge Cartridge::fromJson(const std::string &cartJson) {
        nlohmann::json json = nlohmann::json::parse(cartJson);

        // Parse spritesheets
        std::vector<graphics::Spritesheet> spritesheets;

        // TODO: move this crap to Spritesheet::fromJson
        for (const auto &spritesheetJson : json["spritesheets"]) {
            const auto textureJson = spritesheetJson["texture"];
            const auto width = textureJson["width"].get<uint32_t>();
            const auto height = textureJson["height"].get<uint32_t>();
            const auto data = textureJson["data"].get<std::string>();

            const auto parsedData = api::split(data, ' ');
            const auto parsedBytes = api::convertToBytes(parsedData);

            std::vector<uint32_t> spriteData;
            for (const auto &spriteJson : spritesheetJson["sprites"]) {
                spriteData.push_back(spriteJson["x"].get<uint32_t>());
                spriteData.push_back(spriteJson["y"].get<uint32_t>());
                spriteData.push_back(spriteJson["width"].get<uint32_t>());
                spriteData.push_back(spriteJson["height"].get<uint32_t>());
                spriteData.push_back(spriteJson["flags"].get<uint32_t>());
            }

            spritesheets.push_back(graphics::Spritesheet::fromData(parsedBytes, spriteData, width, height));
        }

        std::vector<collision::Collider> colliders;

        std::cout << "BEFORE COLLIDERS" << std::endl;
        for (const auto& colliderJson : json["colliders"]) {
            colliders.push_back(collision::Collider::fromJson(colliderJson));
        }
        std::cout << "AFTER COLLIDERS" << std::endl;

        return Cartridge(json["code"].get<std::string>(), spritesheets, colliders);
    }

    const std::vector<graphics::Spritesheet> &Cartridge::getSpritesheets() {
        return spritesheets;
    }

    const std::vector<collision::Collider> &Cartridge::getColliders() {
        return colliders;
    }

    graphics::Spritesheet &Cartridge::getSpritesheet(uint32_t index) {
        return spritesheets[index];
    }
} // blipcade