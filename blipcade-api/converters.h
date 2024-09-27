//
// Created by Pavlo Yevsehnieiev
//

#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <string>


namespace blipcade::api {
    std::wstring decodeBeBytesToWstring(const std::vector<uint8_t> &bytes);
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::vector<uint8_t> convertToBytes(const std::vector<std::string>& hexStrings);
} // api
// blipcade

#endif //CONVERTERS_H
