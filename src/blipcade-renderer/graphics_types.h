#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

namespace blipcade::graphics {

    struct Size {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct Rect {
        int32_t x = 0;
        int32_t y = 0;
        int32_t width = 0;
        int32_t height = 0;
    };

    struct Vector2 {
        int32_t x = 0;
        int32_t y = 0;
    };

    // You can add other common graphics-related structs here, like Vector2, Rect, etc.

} // namespace blipcade::graphics

#endif // GRAPHICS_TYPES_H