#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <array>
#include <ostream>

namespace blipcade {
    namespace graphics {
        // struct Color {
        //     uint8_t r;
        //     uint8_t g;
        //     uint8_t b;
        //     uint8_t a;
        //
        //     // Constructors
        //     Color();
        //
        //     Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        //
        //     // Static methods
        //     static Color from_hex(uint32_t hex);
        //
        //     [[nodiscard]] uint32_t to_hex() const;
        //
        //     // Member functions
        //     std::array<uint8_t, 4> to_rgba() const;
        //
        //     Color clone() const;
        //
        //     // Operator overloads
        //     bool operator==(const Color &other) const;
        //
        //     bool operator!=(const Color &other) const;
        //
        //     // Friend functions
        //     friend std::ostream &operator<<(std::ostream &os, const Color &color);
        // };
    }
}
#endif // COLORS_H
