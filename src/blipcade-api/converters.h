//
// Created by Pavlo Yevsehnieiev
//

#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <raylib.h>
#include <string>


namespace blipcade::api {
    std::wstring decodeBeBytesToWstring(const std::vector<uint8_t> &bytes);

    std::vector<std::string> split(const std::string &s, const char delimiter);

    std::vector<uint8_t> convertToBytes(const std::vector<std::string> &hexStrings);

    std::vector<uint8_t> imageToBytes(const std::string &path);

    std::vector<uint8_t> imageToBytes(Image &image);
} // api
// blipcade

#endif //CONVERTERS_H
