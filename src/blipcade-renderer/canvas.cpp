//
// Created by Pavlo Yevsehnieiev
//

#include "canvas.h"

#include <iomanip>
#include <iostream>
#include <raylib.h>

#include "font.h"

#include "palette_shader.h"

namespace blipcade::graphics {
    Canvas::Canvas(const uint32_t width, const uint32_t height): width(width), height(height),
                                                                 palette(std::make_unique<Palette685>()) {
        // paletteShader = LoadShader(nullptr, "palette_shader.frag");
        paletteShader = LoadShaderFromMemory(nullptr, palette_shader_fragmentSource);
        paletteLoc = GetShaderLocation(paletteShader, "palette");

        for (int i = 0; i < 256; ++i) {
            virtualPalette[i] = static_cast<uint8_t>(i);
            colorLookup[i] = palette->get_color(i); // Define this function based on your palette
        }

        updateShaderPalette();
    }

    void Canvas::setTransparentColor(uint8_t color) {
        transparentColor = color;

        // Update the transparent index in the shader
        int transparentIndexLoc = GetShaderLocation(paletteShader, "transparentIndex");
        int transparentIndex = transparentColor;
        SetShaderValue(paletteShader, transparentIndexLoc, &transparentIndex, SHADER_UNIFORM_INT);
    }

    void Canvas::createPaletteTexture() {
        Image paletteImage;
        paletteImage.width = 256;
        paletteImage.height = 1;
        paletteImage.mipmaps = 1;
        paletteImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        paletteImage.data = new Color[256];

        Color *palettePixels = (Color *) paletteImage.data;
        for (int i = 0; i < 256; ++i) {
            uint8_t colorIndex = virtualPalette[i];
            palettePixels[i] = colorLookup[colorIndex];
        }

        paletteTexture = LoadTextureFromImage(paletteImage);
        UnloadImage(paletteImage);
    }

    void Canvas::updatePaletteTexture() {
        Color palettePixels[256];
        for (int i = 0; i < 256; ++i) {
            uint8_t colorIndex = virtualPalette[i];
            palettePixels[i] = colorLookup[colorIndex];
        }

        UpdateTexture(paletteTexture, palettePixels);
    }

    Canvas::~Canvas() {
        UnloadShader(paletteShader);
    }

    Texture2D paletteTexture;

    void Canvas::setPalette(const std::array<uint8_t, 256> &virtualPalette,
                            const std::array<Color, 256> &colorLookup) {
        this->virtualPalette = virtualPalette;
        this->colorLookup = colorLookup;
        updateShaderPalette();
    }

    void Canvas::updateShaderPalette() {
        // Prepare palette data
        float paletteData[256 * 4]; // Each color has 4 components (RGBA)
        for (int i = 0; i < 256; ++i) {
            uint8_t colorIndex = virtualPalette[i];
            Color color = colorLookup[colorIndex];

            paletteData[i * 4 + 0] = color.r / 255.0f;
            paletteData[i * 4 + 1] = color.g / 255.0f;
            paletteData[i * 4 + 2] = color.b / 255.0f;
            paletteData[i * 4 + 3] = color.a / 255.0f;
        }

        // Print the palette data
        // for (int i = 0; i < 256; ++i) {
            // std::cout << std::fixed << std::setprecision(2) << "Color " << i << ": "
                    // << paletteData[i * 4 + 0] << ", "
                    // << paletteData[i * 4 + 1] << ", "
                    // << paletteData[i * 4 + 2] << ", "
                    // << paletteData[i * 4 + 3] << std::endl;
        // }

        SetShaderValueV(paletteShader, paletteLoc, paletteData, SHADER_UNIFORM_VEC4, 256);
    }

    void Canvas::clear() {
        clipRect = {0, 0, 0, 0};
        offsetX = 0;
        offsetY = 0;
        transparentColor = 255;
    }

    void Canvas::setCamera(const int32_t offsetX, const int32_t offsetY) {
        this->offsetX = offsetX;
        this->offsetY = offsetY;
    }

    uint8_t Canvas::getTransparentColor() const {
        return transparentColor;
    }

    void Canvas::fillScreen(const uint8_t color) {
        const auto colorIndex = virtualPalette[color];
        ClearBackground(colorLookup[colorIndex]);
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
        if (!isWithinClippingRect(x, y)) {
            return;
        }

        const auto realColor = colorLookup[virtualPalette[color]];
        DrawPixel(x, y, realColor);
    }

    void Canvas::drawLine(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                          const uint8_t color) const {
        // TODO: clipper
        auto realColor = colorLookup[virtualPalette[color]];
        DrawLine(x0, y0, x1, y1, realColor);
    }

    void Canvas::drawCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius,
                            const uint8_t color) {
        auto realColor = colorLookup[virtualPalette[color]];
        DrawCircleLines(center_x, center_y, radius, realColor);
    }

    void Canvas::drawFilledCircle(const int32_t center_x, const int32_t center_y, const uint32_t radius,
                                  const uint8_t color) {
        auto realColor = colorLookup[virtualPalette[color]];
        DrawCircle(center_x, center_y, radius, realColor);
    }

    void Canvas::drawRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                               const uint8_t color) {
        const auto realColor = colorLookup[virtualPalette[color]];
        DrawRectangleLines(x0, y0, x0 + x1, y0 + y1, realColor);
    }

    void Canvas::drawRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height,
                                const uint8_t color) {
        const auto realColor = colorLookup[virtualPalette[color]];
        DrawRectangleLines(x, y, width, height, realColor);
    }

    void Canvas::drawFilledRectangle(const int32_t x0, const int32_t y0, const int32_t x1, const int32_t y1,
                                     const uint8_t color) {
        const auto realColor = colorLookup[virtualPalette[color]];
        DrawRectangle(x0, y0, x0 + x1, y0 + y1, realColor);
    }

    void Canvas::drawFilledRectangleW(const int32_t x, const int32_t y, const int32_t width, const int32_t height,
                                      const uint8_t color) {
        const auto realColor = colorLookup[virtualPalette[color]];
        DrawRectangle(x, y, width, height, realColor);
    }

    // void Canvas::drawSprite(int32_t x, int32_t y, bool flipX, bool flipY, const Spritesheet &spritesheet,
    //                         uint32_t index) {
    //     const auto spriteData = spritesheet.getSpriteData(index);
    //
    //     const auto [a, b, width, height, flags] = spritesheet.getSprite(index);
    //     const auto spriteWidth = width;
    //     const auto spriteHeight = height;
    //
    //     if (flipX) {
    //         // flip_x(&mut sprite, sprite_width, sprite_height);
    //     }
    //
    //     if (flipY) {
    //         // flip_y(&mut sprite, sprite_width, sprite_height);
    //     }
    //
    //     const auto x1 = x + offsetX;
    //     const auto y1 = y + offsetY;
    //
    //     drawRectangleData(x1, y1, x1 + spriteWidth, y1 + spriteHeight, true, spriteData);
    // }

    void Canvas::drawSprite(int32_t x, int32_t y, bool flipX, bool flipY,
                            const Spritesheet &spritesheet, uint32_t index) {
        const auto &sprite = spritesheet.getSprite(index);

        Rectangle sourceRec = {
            static_cast<float>(sprite.x),
            static_cast<float>(sprite.y),
            static_cast<float>(sprite.width),
            static_cast<float>(sprite.height)
        };

        if (flipX) sourceRec.width *= -1;
        if (flipY) sourceRec.height *= -1;

        Vector2 position = {
            static_cast<float>(x + offsetX),
            static_cast<float>(y + offsetY)
        };

        BeginShaderMode(paletteShader);
        DrawTextureRec(spritesheet.texture, sourceRec, position, WHITE);
        EndShaderMode();
    }

    // TODO: Replace with Raylib
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
            const auto &[clipX, clipY, clipWidth, clipHeight] = clipRect;
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
                const auto *srcRow = &data[srcY * srcWidth];

                for (auto dstX = xStart; dstX < xEnd; ++dstX) {
                    auto srcX = dstX - x0;
                    auto srcColor = srcRow[srcX];

                    if (transparent && srcColor == transparentColor) {
                        continue;
                    }

                    auto dstColor = virtualPalette[srcColor];

                    auto realColor = colorLookup[dstColor];
                    DrawPixel(dstX, dstY, realColor);
                }
            }
            return;
        }

        float zoomInv = 1.0f / static_cast<float>(zoomValue);

        for (auto dstY = yStart; dstY < yEnd; ++dstY) {
            auto srcY = static_cast<size_t>((static_cast<float>(dstY - y0) * zoomInv));
            const auto *srcRow = &data[srcY * srcWidth];

            for (auto dstX = xStart; dstX < xEnd; ++dstX) {
                auto srcX = static_cast<size_t>((static_cast<float>(dstX - x0) * zoomInv));
                auto srcColor = srcRow[srcX];

                if (transparent && srcColor == transparentColor) {
                    continue;
                }

                auto dstColor = virtualPalette[srcColor];
                auto realColor = colorLookup[dstColor];
                DrawPixel(dstX, dstY, realColor);
            }
        }
    }

    void Canvas::drawText(const Font &font, const std::wstring &text, int32_t x, int32_t y,
                          std::optional<uint8_t> color) {
        const auto colorValue = color.value_or(0xef);
        const auto newColorIndex = virtualPalette[colorValue];
        const auto oldColor = virtualPalette[0xef];

        const auto glyphIndexes = font.getTextRun(text);

        const auto oldTransparentColor = transparentColor;
        virtualPalette[0xef] = newColorIndex;
        transparentColor = 0xff;
        setPalette(virtualPalette, colorLookup);

        for (size_t i = 0; i < glyphIndexes.size(); i++) {
            const auto x1 = static_cast<int32_t>(i * font.glyphSize.width) + x;
            drawSprite(x1, y, false, false, font.spritesheet, glyphIndexes[i]);
        }

        virtualPalette[0xef] = oldColor;
        transparentColor = oldTransparentColor;
        setPalette(virtualPalette, colorLookup);
    }
}
