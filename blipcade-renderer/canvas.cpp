//
// Created by Pavlo Yevsehnieiev
//

#include "canvas.h"

namespace blipcade::graphics {
    Canvas::Canvas(const uint32_t width, const uint32_t height): width(width), height(height), palette(std::make_unique<Palette685>()) {
        pixels.resize(width * height);

        std::ranges::fill(pixels, 0x50);

        for (int i = 0; i < 256; i++) {
            virtualPalette[i] = static_cast<uint8_t>(i);
        }

        for (int i = 0; i < 256; i++) {
            colorLookup[i] = palette->get_color(i).to_hex();
        }

        drawPixel(64, 64, 0xfe);
        drawHorizontalLine(0, 100, 100, 0xfe);
        drawVerticalLine(75, 25, 140, 0x58);
        drawLine(0, 0, 60, 120, 0x64);

        drawCircle(100, 100, 50, 0x34);

        drawFilledCircle(20, 20, 10, 0x24);
    }

    Canvas::~Canvas() = default;

    const std::vector<uint8_t> &Canvas::getPixels() const {
        return pixels;
    }

    std::vector<uint32_t> Canvas::getPixelsData() const {

        std::vector<uint32_t> colors;
        colors.reserve(pixels.size());
        for (const auto pixel : pixels) {
            colors.push_back(colorLookup[virtualPalette[pixel]]);
        }

        return colors;
    }

    void Canvas::clear() {
        std::ranges::fill(pixels, 0x0);
        clipRect = {0, 0, 0, 0};
        offsetX = 0;
        offsetY = 0;
        transparentColor = 255;
        // palette->resetColors();
    }

    void Canvas::setCamera(const int32_t offsetX, const int32_t offsetY) {
        this->offsetX = offsetX;
        this->offsetY = offsetY;
    }

    uint8_t Canvas::getTransparentColor() const {
        return transparentColor;
    }

    void Canvas::setClipRect(const Rect rect) {
        const int32_t x = rect.x < 0 ? 0 : rect.x;
        const int32_t y = rect.y < 0 ? 0 : rect.y;
        const int32_t width = rect.width > this->width ? this->width : rect.width;
        const int32_t height = rect.height > this->height ? this->height : rect.height;

        clipRect = {x, y, width, height};
    }

    bool Canvas::isWithinClippingRect(const int32_t x, const int32_t y) const {
        if (clipRect.width == 0 || clipRect.height == 0) {
            return true;
        }

        return x >= clipRect.x && x < clipRect.x + clipRect.width && y >= clipRect.y && y < clipRect.y + clipRect.height;
    }

    void Canvas::drawPixel(const int32_t x, const int32_t y, const uint8_t color) {
        const auto x1 = x + offsetX;
        const auto y1 = y + offsetY;

        if (!isWithinClippingRect(x1, y1)) {
            return;
        }

        if (x1 < 0 || x1 >= static_cast<int32_t>(width) || y1 < 0 || y1 >= static_cast<int32_t>(height)) {
            return;
        }

        const auto index = static_cast<size_t>(x1 + y1 * width);
        pixels[index] = virtualPalette[color];
    }

    void Canvas::drawHorizontalLine(const int32_t x1, const int32_t x2, const int32_t y, const uint8_t color) {
        const auto colorIndex = virtualPalette[color];

        auto xStart = std::min(x1, x2);
        auto xEnd = std::max(x1, x2);

        auto y1 = y;

        if (clipRect.width != 0 && clipRect.height != 0) {
            const auto clipXEnd = clipRect.x + clipRect.width;
            const auto clipYEnd = clipRect.y + clipRect.height;

            if (y1 < clipRect.y || y1 >= clipYEnd) {
                return;
            }

            xStart = std::max(xStart, clipRect.x);
            xEnd = std::min(xEnd, clipXEnd - 1);

            if (xStart > xEnd) {
                return;
            }
        }

        xStart = std::clamp(xStart, 0, static_cast<int32_t>(width - 1));
        xEnd = std::clamp(xEnd, 0, static_cast<int32_t>(width - 1));
        y1 = std::clamp(y1, 0, static_cast<int32_t>(height - 1));

        const auto startIndex = static_cast<size_t>(y1 * width + xStart);
        const auto endIndex = static_cast<size_t>(y1 * width + xEnd);

        // std::ranges::fill(pixels | std::views::drop(startIndex) | std::views::take(endIndex - startIndex + 1), colorIndex);
        // can't use views, C++ 17 doesn't have them
        for (size_t i = startIndex; i <= endIndex; i++) {
            pixels[i] = colorIndex;
        }
    }

    void Canvas::drawVerticalLine(const int32_t x, const int32_t y1, const int32_t y2, const uint8_t color) {
        const auto colorIndex = virtualPalette[color];

        auto x1 = x;

        if (clipRect.width != 0 && clipRect.height != 0) {
            const auto clipXEnd = clipRect.x + clipRect.width;
            const auto clipYEnd = clipRect.y + clipRect.height;

            if (x1 < clipRect.x || x1 >= clipXEnd) {
                return;
            }

            const auto yStart = std::max(y1, clipRect.y);
            const auto yEnd = std::min(y2, clipYEnd - 1);

            if (yStart > yEnd) {
                return;
            }

            x1 = std::clamp(x1, 0, static_cast<int32_t>(width - 1));
        }

        const auto yStart = std::clamp(y1, 0, static_cast<int32_t>(height - 1));
        const auto yEnd = std::clamp(y2, 0, static_cast<int32_t>(height - 1));

        for (int32_t y = yStart; y <= yEnd; y++) {
            const auto index = static_cast<size_t>(y * width + x1);
            pixels[index] = colorIndex;
        }
    }

    void Canvas::drawLine(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1, const uint8_t color) {
        if (x0 == x1) {
            const auto yStart = std::min(y0, y1);
            const auto yEnd = std::max(y0, y1);
            drawVerticalLine(x0, yStart, yEnd, color);
            return;
        }

        if (y0 == y1) {
            const auto xStart = std::min(x0, x1);
            const auto xEnd = std::max(x0, x1);
            drawHorizontalLine(xStart, xEnd, y0, color);
            return;
        }

        auto dx = x1 - x0;
        auto dy = y1 - y0;
        auto x = x0;
        auto y = y0;

        const auto sx = dx > 0 ? 1 : -1;
        const auto sy = dy > 0 ? 1 : -1;

        dx = std::abs(dx);
        dy = std::abs(dy);

        const auto colorIndex = virtualPalette[color];

        if (dx > dy) {
            auto p = 2 * dy - dx;
            const auto two_dy = 2 * dy;
            const auto two_dy_minus_dx = 2 * (dy - dx);

            for (int i = 0; i < dx; i++) {
                if (x >= 0 && x < static_cast<int32_t>(width) && y >= 0 && y < static_cast<int32_t>(height)) {
                    const auto index = static_cast<size_t>(y * width + x);
                    pixels[index] = colorIndex;
                }

                if (p < 0) {
                    p += two_dy;
                } else {
                    y += sy;
                    p += two_dy_minus_dx;
                }

                x += sx;
            }
        } else {
            auto p = 2 * dx - dy;
            const auto two_dx = 2 * dx;
            const auto two_dx_minus_dy = 2 * (dx - dy);

            for (int i = 0; i < dy; i++) {
                if (x >= 0 && x < static_cast<int32_t>(width) && y >= 0 && y < static_cast<int32_t>(height)) {
                    const auto index = static_cast<size_t>(y * width + x);
                    pixels[index] = colorIndex;
                }

                if (p < 0) {
                    p += two_dx;
                } else {
                    x += sx;
                    p += two_dx_minus_dy;
                }

                y += sy;
            }
        }
    }

    void Canvas::drawCirclePoints(Canvas &canvas, const int32_t cx, const int32_t cy, const int32_t x, const int32_t y, const uint8_t color) {
        canvas.drawPixel(cx + x, cy + y, color);
        canvas.drawPixel(cx - x, cy + y, color);
        canvas.drawPixel(cx + x, cy - y, color);
        canvas.drawPixel(cx - x, cy - y, color);
        canvas.drawPixel(cx + y, cy + x, color);
        canvas.drawPixel(cx - y, cy + x, color);
        canvas.drawPixel(cx + y, cy - x, color);
        canvas.drawPixel(cx - y, cy - x, color);
    }

    void Canvas::drawCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius, const uint8_t color) {
        auto x = static_cast<int32_t>(radius);
        auto y = 0;
        auto decision = 1 - x;

        while (x >= y) {
            drawCirclePoints(*this, center_x, center_y, x, y, color);
            y += 1;
            if (decision <= 0) {
                decision += 2 * y + 1;
            } else {
                x -= 1;
                decision += 2 * (y - x) + 1;
            }
        }
    }

    /*
    *pub fn draw_filled_circle(&mut self, center_x: i32, center_y: i32, radius: u32, color: u8) {
        let r = radius as i32;
        let mut x = r;
        let mut y = 0;
        let mut decision = 1 - r;

        let color = self.palette.get_virtual_color_index(color);

        while x >= y {
            self.draw_horizontal_line(center_x - x, center_x + x, center_y + y, color);
            self.draw_horizontal_line(center_x - x, center_x + x, center_y - y, color);

            if x > y {
                self.draw_horizontal_line(center_x - y, center_x + y, center_y + x, color);
                self.draw_horizontal_line(center_x - y, center_x + y, center_y - x, color);
            }

            y += 1;
            if decision <= 0 {
                decision += 2 * y + 1;
            } else {
                x -= 1;
                decision += 2 * (y - x) + 1;
            }
        }
    }
     */

    void Canvas::drawFilledCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius, const uint8_t color) {
        const auto r = static_cast<int32_t>(radius);
        auto x = r;
        auto y = 0;
        auto decision = 1 - r;

        const auto colorIndex = virtualPalette[color];

        while (x >= y) {
            drawHorizontalLine(center_x - x, center_x + x, center_y + y, colorIndex);
            drawHorizontalLine(center_x - x, center_x + x, center_y - y, colorIndex);

            if (x > y) {
                drawHorizontalLine(center_x - y, center_x + y, center_y + x, colorIndex);
                drawHorizontalLine(center_x - y, center_x + y, center_y - x, colorIndex);
            }

            y += 1;
            if (decision <= 0) {
                decision += 2 * y + 1;
            } else {
                x -= 1;
                decision += 2 * (y - x) + 1;
            }
        }
    }

    void Canvas::drawRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1, const uint8_t color) {
        drawLine(x0, y0, x1, y0, color);
        drawLine(x1, y0, x1, y1, color);
        drawLine(x1, y1, x0, y1, color);
        drawLine(x0, y1, x0, y0, color);
    }

    void Canvas::drawRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height, const uint8_t color) {
        const auto x0 = x;
        const auto y0 = y;
        const auto x1 = x + width - 1;
        const auto y1 = y + height - 1;

        drawRectangle(x0, y0, x1, y1, color);
    }

    void Canvas::drawFilledRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1, const uint8_t color) {
        const auto xStart = std::clamp(x0, 0, static_cast<int32_t>(width - 1));
        const auto xEnd = std::clamp(x1, 0, static_cast<int32_t>(width - 1));
        const auto yStart = std::clamp(y0, 0, static_cast<int32_t>(height - 1));
        const auto yEnd = std::clamp(y1, 0, static_cast<int32_t>(height - 1));

        const auto colorIndex = virtualPalette[color];

        for (int32_t y = yStart; y <= yEnd; y++) {
            drawHorizontalLine(xStart, xEnd, y, colorIndex);
        }
    }

    void Canvas::drawFilledRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height, const uint8_t color) {
        const auto x0 = x;
        const auto y0 = y;
        const auto x1 = x + width - 1;
        const auto y1 = y + height - 1;

        drawFilledRectangle(x0, y0, x1, y1, color);
    }

    
}
