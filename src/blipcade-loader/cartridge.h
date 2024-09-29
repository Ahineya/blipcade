//
// Created by Pavlo Yevsehnieiev
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <string>

namespace blipcade {

class Cartridge {
public:
    Cartridge(std::string code);

    ~Cartridge();

    static Cartridge fromJson(const std::string &cartJson);

    [[nodiscard]] std::string getCode() const {
        return code;
    }
private:
    std::string code;
};

} // blipcade

#endif //CARTRIDGE_H
