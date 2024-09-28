//
// Created by Pavlo Yevsehnieiev
//

#include "canvas.h"

#include <iostream>

#include "font.h"

namespace blipcade::graphics {
    Canvas::Canvas(const uint32_t width, const uint32_t height): width(width), height(height),
                                                                 palette(std::make_unique<Palette685>()) {
        pixels.resize(width * height);
        std::cout << "Canvas constructed at " << this << " with pixels at " << pixels.data() << std::endl;

        std::ranges::fill(pixels, 0x00);

        for (int i = 0; i < 256; i++) {
            virtualPalette[i] = static_cast<uint8_t>(i);
        }

        for (int i = 0; i < 256; i++) {
            colorLookup[i] = palette->get_color(i).to_hex();
        }

        transparentColor = 0xff;

        // drawPixel(64, 64, 0xfe);
        // drawHorizontalLine(0, 100, 100, 0xfe);
        // drawVerticalLine(75, 25, 140, 0x58);
        // drawLine(0, 0, 60, 120, 0x64);
        //
        // drawCircle(100, 100, 50, 0x34);
        //
        // drawFilledCircle(20, 20, 10, 0x24);
    }

    Canvas::~Canvas() = default;

    const std::vector<uint8_t> &Canvas::getPixels() const {
        return pixels;
    }

    std::vector<uint32_t> Canvas::getPixelsData() const {
        std::vector<uint32_t> colors;
        colors.reserve(pixels.size());
        for (const auto pixel: pixels) {
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

        return x >= clipRect.x && x < clipRect.x + clipRect.width && y >= clipRect.y && y < clipRect.y + clipRect.
               height;
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

    void Canvas::drawCirclePoints(Canvas &canvas, const int32_t cx, const int32_t cy, const int32_t x, const int32_t y,
                                  const uint8_t color) {
        canvas.drawPixel(cx + x, cy + y, color);
        canvas.drawPixel(cx - x, cy + y, color);
        canvas.drawPixel(cx + x, cy - y, color);
        canvas.drawPixel(cx - x, cy - y, color);
        canvas.drawPixel(cx + y, cy + x, color);
        canvas.drawPixel(cx - y, cy + x, color);
        canvas.drawPixel(cx + y, cy - x, color);
        canvas.drawPixel(cx - y, cy - x, color);
    }

    void Canvas::drawCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius,
                            const uint8_t color) {
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

    void Canvas::drawFilledCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius,
                                  const uint8_t color) {
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

    void Canvas::drawRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                               const uint8_t color) {
        drawLine(x0, y0, x1, y0, color);
        drawLine(x1, y0, x1, y1, color);
        drawLine(x1, y1, x0, y1, color);
        drawLine(x0, y1, x0, y0, color);
    }

    void Canvas::drawRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height,
                                const uint8_t color) {
        const auto x0 = x;
        const auto y0 = y;
        const auto x1 = x + width - 1;
        const auto y1 = y + height - 1;

        drawRectangle(x0, y0, x1, y1, color);
    }

    void Canvas::drawFilledRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                                     const uint8_t color) {
        const auto xStart = std::clamp(x0, 0, static_cast<int32_t>(width - 1));
        const auto xEnd = std::clamp(x1, 0, static_cast<int32_t>(width - 1));
        const auto yStart = std::clamp(y0, 0, static_cast<int32_t>(height - 1));
        const auto yEnd = std::clamp(y1, 0, static_cast<int32_t>(height - 1));

        const auto colorIndex = virtualPalette[color];

        for (int32_t y = yStart; y <= yEnd; y++) {
            drawHorizontalLine(xStart, xEnd, y, colorIndex);
        }
    }

    void Canvas::drawFilledRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height,
                                      const uint8_t color) {
        const auto x0 = x;
        const auto y0 = y;
        const auto x1 = x + width - 1;
        const auto y1 = y + height - 1;

        drawFilledRectangle(x0, y0, x1, y1, color);
    }

    void Canvas::drawSprite(int32_t x, int32_t y, bool flipX, bool flipY, const Spritesheet &spritesheet,
                            uint32_t index) {
        const auto spriteData = spritesheet.getSpriteData(index);

        const auto [a, b, width, height, flags] = spritesheet.getSprite(index);
        const auto spriteWidth = width;
        const auto spriteHeight = height;

        if (flipX) {
            // flip_x(&mut sprite, sprite_width, sprite_height);
        }

        if (flipY) {
            // flip_y(&mut sprite, sprite_width, sprite_height);
        }

        const auto x1 = x + offsetX;
        const auto y1 = y + offsetY;

        drawRectangleData(x1, y1, x1 + spriteWidth, y1 + spriteHeight, true, spriteData);
    }

    // TODO: Port optimized drawRectangleData from Rust
    /*
     *    pub fn draw_rectangle_data(
        &self,
        x0: i32,
        y0: i32,
        x1: i32,
        y1: i32,
        draw_with_transparency: bool,
        data: &[u8],
        zoom: u8,
    ) {
        let src_width = (x1 - x0) as usize;
        let src_height = (y1 - y0) as usize;

        if data.len() != src_width * src_height {
            eprintln!("Data length does not match rectangle size");
            return;
        }

        let zoom = zoom.max(1) as usize;
        let dst_width = src_width * zoom;
        let dst_height = src_height * zoom;

        let mut x_start = x0.max(0).min(self.width as i32) as usize;
        let mut x_end = (x0 + dst_width as i32).max(0).min(self.width as i32) as usize;
        let mut y_start = y0.max(0).min(self.height as i32) as usize;
        let mut y_end = (y0 + dst_height as i32).max(0).min(self.height as i32) as usize;

        if let Some((clip_x, clip_y, clip_width, clip_height)) = self.clipping_rect {
            x_start = x_start.max(clip_x as usize);
            x_end = x_end.min((clip_x + clip_width) as usize);
            y_start = y_start.max(clip_y as usize);
            y_end = y_end.min((clip_y + clip_height) as usize);
        }

        if x_start >= x_end || y_start >= y_end {
            return;
        }

        // Handle zoom == 1 case separately
        if zoom == 1 {
            for dst_y in y_start..y_end {
                let src_y = dst_y as i32 - y0;
                let dst_start = dst_y * self.width as usize;
                let src_row = &data[src_y as usize * src_width..(src_y as usize + 1) * src_width];

                for dst_x in x_start..x_end {
                    let src_x = dst_x as i32 - x0;
                    let src_color = src_row[src_x as usize];

                    if draw_with_transparency && src_color == self.transparent_color {
                        continue;
                    }

                    let dst_color = self.palette.get_virtual_color_index(src_color);
                    self.pixel_buffer.borrow_mut()[dst_start + dst_x] = dst_color;
                }
            }
            return;
        }

        let zoom_inv = 1.0 / zoom as f32;

        for dst_y in y_start..y_end {
            let src_y = ((dst_y as i32 - y0) as f32 * zoom_inv) as usize;
            let dst_start = dst_y * self.width as usize;
            let src_row = &data[src_y * src_width..(src_y + 1) * src_width];

            for dst_x in x_start..x_end {
                let src_x = ((dst_x as i32 - x0) as f32 * zoom_inv) as usize;
                let src_color = src_row[src_x];

                if draw_with_transparency && src_color == self.transparent_color {
                    continue;
                }

                let dst_color = self.palette.get_virtual_color_index(src_color);
                self.pixel_buffer.borrow_mut()[dst_start + dst_x] = dst_color;
            }
        }
    }
     */
    void Canvas::drawRectangleData(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                               const bool transparent, const std::vector<uint8_t> &data) {
    auto srcWidth = x1 - x0;
    auto srcHeight = y1 - y0;

    if (data.size() != static_cast<size_t>(srcWidth * srcHeight)) {
        std::cerr << "Data length does not match rectangle size" << std::endl;
        return;
    }

    auto zoom = 1;

    auto zoomValue = std::max(static_cast<size_t>(zoom), static_cast<size_t>(1));
    auto dstWidth = static_cast<size_t>(srcWidth) * zoomValue;
    auto dstHeight = static_cast<size_t>(srcHeight) * zoomValue;

    auto xStart = std::clamp(x0, 0, static_cast<int32_t>(width));
    auto xEnd = std::clamp(x0 + static_cast<int32_t>(dstWidth), 0, static_cast<int32_t>(width));
    auto yStart = std::clamp(y0, 0, static_cast<int32_t>(height));
    auto yEnd = std::clamp(y0 + static_cast<int32_t>(dstHeight), 0, static_cast<int32_t>(height));

    if (clipRect.x != 0 || clipRect.y != 0 || clipRect.width != 0 || clipRect.height != 0) {
        const auto& [clipX, clipY, clipWidth, clipHeight] = clipRect;
        xStart = std::max(xStart, static_cast<int32_t>(clipX));
        xEnd = std::min(xEnd, static_cast<int32_t>(clipX + clipWidth));
        yStart = std::max(yStart, static_cast<int32_t>(clipY));
        yEnd = std::min(yEnd, static_cast<int32_t>(clipY + clipHeight));
    }

    if (xStart >= xEnd || yStart >= yEnd) {
        return;
    }

    // Handle zoom == 1 case separately
    if (zoomValue == 1) {
        for (auto dstY = yStart; dstY < yEnd; ++dstY) {
            auto srcY = dstY - y0;
            auto dstStart = dstY * width;
            // &data[src_y as usize * src_width..(src_y as usize + 1) * src_width];
            const auto* srcRow = &data[srcY * srcWidth];

            for (auto dstX = xStart; dstX < xEnd; ++dstX) {
                auto srcX = dstX - x0;
                auto srcColor = srcRow[srcX];

                if (transparent && srcColor == transparentColor) {
                    continue;
                }

                auto dstColor = virtualPalette[srcColor];
                pixels[dstStart + dstX] = dstColor;
            }
        }
        return;
    }

    float zoomInv = 1.0f / static_cast<float>(zoomValue);

    for (auto dstY = yStart; dstY < yEnd; ++dstY) {
        auto srcY = static_cast<size_t>((static_cast<float>(dstY - y0) * zoomInv));
        auto dstStart = dstY * width;
        const auto* srcRow = &data[srcY * srcWidth];

        for (auto dstX = xStart; dstX < xEnd; ++dstX) {
            auto srcX = static_cast<size_t>((static_cast<float>(dstX - x0) * zoomInv));
            auto srcColor = srcRow[srcX];

            if (transparent && srcColor == transparentColor) {
                continue;
            }

            auto dstColor = virtualPalette[srcColor];
            pixels[dstStart + dstX] = dstColor;
        }
    }
}

    void Canvas::drawText(const Font &font, const std::wstring &text, int32_t x, int32_t y, std::optional<uint8_t> color) {
        const auto colorValue = color.value_or(0xef);
        const auto newColorIndex = virtualPalette[colorValue];
        const auto oldColor = virtualPalette[0xef];

        const auto glyphIndexes = font.getTextRun(text);

        const auto oldTransparentColor = transparentColor;
        virtualPalette[0xef] = newColorIndex;
        transparentColor = 0xff;

        for (size_t i = 0; i < glyphIndexes.size(); i++) {
            const auto x1 = static_cast<int32_t>(i * font.glyphSize.width) + x;
            drawSprite(x1, y, false, false, font.spritesheet, glyphIndexes[i]);
        }

        virtualPalette[0xef] = oldColor;
        transparentColor = oldTransparentColor;
    }
}
