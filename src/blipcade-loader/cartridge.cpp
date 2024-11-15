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
    std::unordered_map<std::string, graphics::Spritesheet> spritesheets,
    std::unordered_map<std::string, collision::Collider> colliders,
    std::unordered_map<std::string, collision::NavMesh> navmeshes
)
    : code(std::move(code)),
      spritesheets(std::move(spritesheets)),
      colliders(std::move(colliders)),
      navmeshes(std::move(navmeshes))
    {}
    Cartridge::~Cartridge() {
        spritesheets.clear();
    }

    Cartridge Cartridge::fromJson(const std::string &cartJson) {
        nlohmann::json json = nlohmann::json::parse(cartJson);

        // Parse spritesheets
        std::unordered_map<std::string, graphics::Spritesheet> spritesheets;

        auto int i = 0;

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

            // spritesheets.push_back(graphics::Spritesheet::fromData(parsedBytes, spriteData, width, height));
            spritesheets.insert({std::to_string(i), graphics::Spritesheet::fromData(parsedBytes, spriteData, width, height)});
            i++;
        }

        std::unordered_map<std::string, collision::Collider> colliders;
        i = 0;
        std::cout << "BEFORE COLLIDERS" << std::endl;
        for (const auto& colliderJson : json["colliders"]) {
            // colliders.push_back(collision::Collider::fromJson(colliderJson));
            colliders.insert({std::to_string(i), collision::Collider::fromJson(colliderJson)});
            i++;
        }
        std::cout << "AFTER COLLIDERS" << std::endl;

        std::cout << "BEFORE NAVMESHES" << std::endl;
        std::unordered_map<std::string, collision::NavMesh> navmeshes;
        i = 0;

        for (const auto& navmeshJson : json["navmeshes"]) {
            navmeshes.insert({std::to_string(i), collision::NavMesh::fromJson(navmeshJson)});
            i++;
        }

        // Log how many navmeshes are in the cartridge
        std::cout << "NAVMESHES SIZE: " << navmeshes.size() << std::endl;

        std::cout << "AFTER NAVMESHES" << std::endl;

        return Cartridge(json["code"].get<std::string>(), spritesheets, colliders, navmeshes);
    }

    const std::unordered_map<std::string, graphics::Spritesheet> &Cartridge::getSpritesheets() {
        return spritesheets;
    }

    const std::unordered_map<std::string, collision::Collider> &Cartridge::getColliders() {
        return colliders;
    }

    const std::unordered_map<std::string, collision::NavMesh> &Cartridge::getNavmeshes() {
        return navmeshes;
    }
} // blipcade