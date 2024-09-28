#ifndef CANVAS_H
#define CANVAS_H
#include <cstdint>
#include <palette685.h>
#include <vector>

#include "font.h"
#include "graphics_types.h"
#include "spritesheet.h"

namespace blipcade::graphics {
    struct Font;

    class Canvas {
    public:
        Canvas(uint32_t width, uint32_t height);

        ~Canvas();

        [[nodiscard]] const std::vector<uint8_t> &getPixels() const;

        std::vector<uint32_t> getPixelsData() const;

        void setOffset(int32_t offsetX, int32_t offsetY);

        void setTransparentColor(uint8_t index);

        void fillScreen(uint8_t color);

        void setClipRect(Rect rect);

        bool isWithinClippingRect(int32_t x, int32_t y) const;

        void drawPixel(int32_t x, int32_t y, uint8_t color);

        void drawHorizontalLine(int32_t x1, int32_t x2, int32_t y, uint8_t color);

        void drawVerticalLine(int32_t x, int32_t y1, int32_t y2, uint8_t color);

        void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);

        void drawCirclePoints(Canvas &canvas, int32_t cx, int32_t cy, int32_t x, int32_t y, uint8_t color);

        void drawCircle(int32_t center_x, int32_t center_y, uint32_t radius, uint8_t color);

        void drawFilledCircle(int32_t center_x, int32_t center_y, uint32_t radius, uint8_t color);

        void drawRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);

        void drawRectangleW(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);

        void drawFilledRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);

        void drawFilledRectangleW(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);

        // pub fn draw_sprite(&self, x: i32, y: i32, flip_x: bool, flip_y: bool, spritesheet: &Spritesheet, index: usize) {
        //     let mut sprite = spritesheet.get_sprite_data(index);
        //     let sprite_width = spritesheet.get_sprite(index).width as u32;
        //     let sprite_height = spritesheet.get_sprite(index).height as u32;
        //
        //     if flip_x {
        //         Self::flip_x(&mut sprite, sprite_width, sprite_height);
        //     }
        //
        //     if flip_y {
        //         Self::flip_y(&mut sprite, sprite_width, sprite_height);
        //     }
        //
        //     // Apply offset
        //     let x = x + self.offset.x as i32;
        //     let y = y + self.offset.y as i32;
        //
        //     self.draw_rectangle_data(x, y, x + sprite_width as i32, y + sprite_height as i32, true, &sprite, 1);
        // }

        void drawSprite(int32_t x, int32_t y, bool flipX, bool flipY,
                        const Spritesheet &spritesheet,
                        uint32_t index);

        void drawRectangleData(int32_t x0, int32_t y0, int32_t x1, int32_t y1, bool transparent,
                               const std::vector<uint8_t> &data);

        void drawText(const Font &font, const std::wstring &text, int32_t x, int32_t y, std::optional<uint8_t> color);

        void clear();

        void setCamera(int32_t offsetX, int32_t offsetY);

        uint8_t getTransparentColor() const;

    private:
        uint32_t width = 0;
        uint32_t height = 0;

        std::array<uint32_t, 256> colorLookup{};
        std::vector<uint8_t> pixels;
        std::array<uint8_t, 256> virtualPalette{};
        std::unique_ptr<Palette685> palette;

        int32_t offsetX = 0;
        int32_t offsetY = 0;

        uint8_t transparentColor = 0;

        Rect clipRect = {0, 0, 0, 0};
    };
}


#endif //CANVAS_H
