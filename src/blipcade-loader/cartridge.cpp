//
// Created by Pavlo Yevsehnieiev
//

#include "cartridge.h"

#include <converters.h>
#include <iostream>
#include <spritesheet.h>
#include <nlohmann/json.hpp>


/*
 Demo json for reference:
{
  "name": "testcart",
  "code": "../carts-js/lib/testcart.js",
  "spritesheets": [
    {
      "texture": {
        "width": 13,
        "height": 13,
        "data": "00 FF FF FF FF FF FF FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF FF FF FF FF 00 EF 00 FF FF FF FF FF FF FF FF FF FF 00 EF EF 00 FF FF FF FF FF FF FF FF FF 00 EF EF EF 00 FF FF FF FF FF FF FF FF 00 EF EF EF EF 00 FF FF FF FF FF FF FF 00 EF EF EF EF EF 00 FF FF FF FF FF FF 00 EF EF EF EF EF EF 00 FF FF FF FF FF 00 EF EF EF EF 00 00 00 FF FF FF FF FF 00 EF 00 EF EF 00 FF FF FF FF FF FF FF 00 00 FF 00 EF EF 00 FF FF FF FF FF FF FF FF FF 00 EF EF 00 FF FF FF FF FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF"
      },
      "sprites": [
        {
          "x": 0,
          "y": 0,
          "width": 8,
          "height": 13,
          "flags": 0
        }
      ]
    }
  ]
}
 */

namespace blipcade {
    Cartridge::Cartridge(
        std::string code,
        const std::vector<graphics::Spritesheet>& spritesheets
    ): code(std::move(code)), spritesheets(spritesheets) {}
    Cartridge::~Cartridge() {
        spritesheets.clear();
    }

    Cartridge Cartridge::fromJson(const std::string &cartJson) {
        nlohmann::json json = nlohmann::json::parse(cartJson);

        // Parse spritesheets
        std::vector<graphics::Spritesheet> spritesheets;

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

                std::cout << "Adding sprite: x: " << spriteJson["x"].get<uint32_t>() << ", y: " << spriteJson["y"].get<uint32_t>() << ", width: " << spriteJson["width"].get<uint32_t>() << ", height: " << spriteJson["height"].get<uint32_t>() << ", flags: " << std::bitset<8>(spriteJson["flags"].get<uint8_t>()) << "\n";
            }

            spritesheets.push_back(graphics::Spritesheet::fromData(parsedBytes, spriteData, width, height));
        }

        std::cout << "Loaded " << spritesheets.size() << " spritesheets" << std::endl;

        return Cartridge(json["code"].get<std::string>(), spritesheets);
    }

    const std::vector<graphics::Spritesheet> &Cartridge::getSpritesheets() {
        return spritesheets;
    }

    graphics::Spritesheet &Cartridge::getSpritesheet(uint32_t index) {
        return spritesheets[index];
    }
} // blipcade