//
// Created by Pavlo Yevsehnieiev
//

#include "converters.h"

#include <sstream>
#include <vector>

namespace blipcade::api {
    std::wstring decodeBeBytesToWstring(const std::vector<uint8_t> &bytes) {
        std::wstring result;
        for (size_t i = 0; i < bytes.size(); i += 4) {
            if (i + 4 > bytes.size()) {
                break;
            }
            std::array chunk = {bytes[i + 3], bytes[i + 2], bytes[i + 1], bytes[i]};

            uint32_t scalar_value = 0;
            for (size_t j = 0; j < 4; j++) {
                scalar_value = (scalar_value << 8) | chunk[j];
            }

            if (scalar_value < 0x10000) {
            result.push_back(scalar_value);
            } else {
            result.push_back(0xFFFD); // Unicode replacement character for invalid scalar values
            }
        }
        return result;
    }

    std::vector<std::string> split(const std::string& s, const char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

    std::vector<uint8_t> convertToBytes(const std::vector<std::string>& hexStrings) {
        std::vector<uint8_t> bytes;
        for (const auto& hex : hexStrings) {

            uint8_t byte = std::stoul(hex, nullptr, 16);
            bytes.push_back(byte);
        }
        return bytes;
    }
} // api
// blipcade
