//
// Created by Pavlo Yevsehnieiev
//

#include "cartridge.h"

#include <iostream>
#include <nlohmann/json.hpp>

namespace blipcade {
    Cartridge::Cartridge() = default;
    Cartridge::~Cartridge() = default;

    Cartridge Cartridge::fromJson(const std::string &cartJson) {
        nlohmann::json json = nlohmann::json::parse(cartJson);

        std::cout << json.dump() << std::endl;

        return {};
    }
} // blipcade