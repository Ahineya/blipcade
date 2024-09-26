#include "color.h"

namespace blipcade {
    namespace graphics {
        // Default constructor
        Color::Color() : r(0), g(0), b(0), a(255) {}

        // Parameterized constructor
        Color::Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
            : r(red), g(green), b(blue), a(alpha) {}

        // Static method to create Color from hex
        Color Color::from_hex(const uint32_t hex) {
            uint8_t red   = (hex >> 24) & 0xFF;
            uint8_t green = (hex >> 16) & 0xFF;
            uint8_t blue  = (hex >> 8) & 0xFF;
            return {red, green, blue, 255};
        }

        // uint32_t Color::to_hex() const { // Should return rgba as hex
            // return (r << 24) | (g << 16) | (b << 8) | a;
        // }

        uint32_t Color::to_hex() const {
            return (a << 24) | (b << 16) | (g << 8) | r;
        }

        // Method to return RGBA as an array
        std::array<uint8_t, 4> Color::to_rgba() const {
            return { r, g, b, a };
        }

        // Clone method
        Color Color::clone() const {
            return { r, g, b, a };
        }

        // Equality operators
        bool Color::operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        bool Color::operator!=(const Color& other) const {
            return !(*this == other);
        }

        // Stream insertion operator for easy printing
        std::ostream& operator<<(std::ostream& os, const Color& color) {
            os << "Color(r=" << static_cast<int>(color.r)
               << ", g=" << static_cast<int>(color.g)
               << ", b=" << static_cast<int>(color.b)
               << ", a=" << static_cast<int>(color.a) << ")";
            return os;
        }
    }
}