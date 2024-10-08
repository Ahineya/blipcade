//
// Created by Pavlo Yevsehnieiev
//

#include "spritesheet.h"

#include <converters.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/json.hpp>
#include <__filesystem/filesystem_error.h>


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

            spritesheet.sprites.push_back(Sprite{x, y, sprWidth, sprHeight, flags});
        }

        spritesheet.createTexture();

        return spritesheet;
    }

    Spritesheet Spritesheet::fromResource(const std::string &resourcePath, const std::string &projectDir) {
        std::cout << "Loading spritesheet from resource: " << resourcePath << "\n";

        const auto path = resourcePath.substr(6);

        std::filesystem::path fullPath = std::filesystem::path(projectDir) / path;
        nlohmann::json spritesheetJson;

        std::ifstream spritesheetStream(fullPath.lexically_normal());
        spritesheetStream >> spritesheetJson;

        const auto textureJson = spritesheetJson["texture"];

        unsigned width = 0;
        unsigned height = 0;
        std::vector<uint8_t> parsedBytes;

        // Check if "image" is present in the texture
        if (textureJson.contains("image")) {
            // Here we want to load an image, and convert it to a spritesheet
            const auto imagePath = textureJson["image"].get<std::string>();

            // image path is relative to the resource file, so we need to get the directory of the resource file
            const auto resourceDir = fullPath.parent_path();
            const auto imageFullPath = resourceDir / imagePath;

            Image img = LoadImage(imageFullPath.c_str());
            width = img.width;
            height = img.height;
            parsedBytes = api::imageToBytes(img);
        } else {
            width = textureJson["width"].get<uint32_t>();
            height = textureJson["height"].get<uint32_t>();
            const auto data = textureJson["data"].get<std::string>();

            const auto parsedData = api::split(data, ' ');
            parsedBytes = api::convertToBytes(parsedData);
        }

        std::vector<uint32_t> spriteData;
        for (const auto &spriteJson : spritesheetJson["sprites"]) {
            spriteData.push_back(spriteJson["x"].get<uint32_t>());
            spriteData.push_back(spriteJson["y"].get<uint32_t>());
            spriteData.push_back(spriteJson["width"].get<uint32_t>());
            spriteData.push_back(spriteJson["height"].get<uint32_t>());
            spriteData.push_back(spriteJson["flags"].get<uint32_t>());
        }

        std::cout << "Spritesheet loaded: width: " << width << ", height: " << height << "\n";

        return fromData(parsedBytes, spriteData, width, height);
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
        sprites.push_back(Sprite{x, y, width, height, flags});
    }

    const Sprite &Spritesheet::getSprite(const uint32_t index) const {
        if (index < sprites.size()) {
            return sprites[index];
        }

        std::cerr << "Invalid sprite index: " << index << "\n";

        throw std::runtime_error("Invalid sprite index");
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
