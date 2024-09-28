//
// Created by Pavlo Yevsehnieiev
//

#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <string>

namespace blipcade {

class Cartridge {
public:
    Cartridge();
    ~Cartridge();

    static Cartridge fromJson(const std::string &cartJson);
private:
};

} // blipcade

#endif //CARTRIDGE_H
