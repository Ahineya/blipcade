//
// Created by Pavlo Yevsehnieiev
//

#include "canvas.h"

#include <iostream>
#include <raylib.h>
#include <raymath.h>

#include "font.h"

#include "palette_shader.h"
#include "lighting_shader.h"
#include "lighting_shader_vert.h"

namespace blipcade::graphics {
    Canvas::Canvas(const uint32_t width, const uint32_t height): width(width), height(height),
    palette(std::make_unique<Palette685>()), lightingRender(LoadRenderTexture(width, height)),
lightEffects()  {
        // paletteShader = LoadShader(nullptr, "palette_shader.frag");
        paletteShader = LoadShaderFromMemory(nullptr, palette_shader_fragmentSource);
        paletteLoc = GetShaderLocation(paletteShader, "palette");

        // Load lighting shader
        lightingShader = LoadShaderFromMemory(nullptr, lighting_shader_fragmentSource);

        // Set baseTexture uniform to the paletteTexture
        // int baseTextureLoc = GetShaderLocation(lightingShader, "baseTexture");
        // SetShaderValueTexture(lightingShader, baseTextureLoc, paletteTexture);

        for (int i = 0; i < 256; ++i) {
            virtualPalette[i] = static_cast<uint8_t>(i);
            colorLookup[i] = palette->get_color(i); // Define this function based on your palette
        }

        updateShaderPalette();
        lightingRender = LoadRenderTexture(width, height);

        // Set the MVP matrix
        Matrix mvp = MatrixIdentity(); // Replace with your actual MVP matrix
        int mvpLoc = GetShaderLocation(lightingShader, "mvp");
        SetShaderValueMatrix(lightingShader, mvpLoc, mvp);

        tintColorLoc = GetShaderLocation(lightingShader, "tintColor");
        opacityLoc = GetShaderLocation(lightingShader, "opacity");
        baseTextureLoc = GetShaderLocation(lightingShader, "baseTexture");
        maskTextureLoc = GetShaderLocation(lightingShader, "maskTexture");

        // std::cout << "Canvas created with width: " << width << ", height: " << height << std::endl;
        ClearBackground(BLANK);
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
        UnloadShader(lightingShader);
        UnloadTexture(lightingRender.texture);
    }

    Texture2D paletteTexture;

    void Canvas::setPalette(const std::array<uint8_t, 256> &virtualPalette,
                            const std::array<Color, 256> &colorLookup) {
        this->virtualPalette = virtualPalette;
        this->colorLookup = colorLookup;
        updateShaderPalette();
    }

    void Canvas::updateShaderPalette() {
        float paletteData[256 * 4]; // Each color has 4 components (RGBA)
        for (int i = 0; i < 256; ++i) {
            uint8_t colorIndex = virtualPalette[i];
            Color color = colorLookup[colorIndex];

            paletteData[i * 4 + 0] = color.r / 255.0f;
            paletteData[i * 4 + 1] = color.g / 255.0f;
            paletteData[i * 4 + 2] = color.b / 255.0f;
            paletteData[i * 4 + 3] = color.a / 255.0f;
        }

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
        DrawTexturePro(spritesheet.texture, sourceRec, {position.x, position.y, sourceRec.width, sourceRec.height}, {0, 0}, 0, WHITE);
        EndShaderMode();
    }

    void Canvas::drawSpriteEx(int32_t x, int32_t y, bool flipX, bool flipY, float scale, float originX, float originY,
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

        // Vector2 origin = {originX, originY};
        Rectangle destRect = {position.x, position.y, sourceRec.width * scale, sourceRec.height * scale};
        Vector2 origin = {abs(originX * destRect.width), abs(originY * destRect.height)};

        // Logging.
        // std::cout << "x: " << x << ", y: " << y << ", offsetX: " << offsetX << ", offsetY: " << offsetY << std::endl;
        // std::cout << "sourceRec: " << sourceRec.x << ", " << sourceRec.y << ", " << sourceRec.width << ", " << sourceRec.height << std::endl;
        // std::cout << "position: " << position.x << ", " << position.y << std::endl;
        // std::cout << "destRect: " << destRect.x << ", " << destRect.y << ", " << destRect.width << ", " << destRect.height << std::endl;
        // std::cout << "origin: " << origin.x << ", " << origin.y << std::endl;

        BeginShaderMode(paletteShader);
        DrawTexturePro(spritesheet.texture, sourceRec, destRect, origin, 0, WHITE);
        EndShaderMode();
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

    void Canvas::addLightEffect(const std::string &name, const LightEffect &effect) {
        lightEffects[name] = effect;
    }

    void Canvas::removeLightEffect(const std::string &name) {
        lightEffects.erase(name);
    }

    void Canvas::updateLightEffect(const std::string &name, const LightEffect &effect) {
        if (lightEffects.contains(name)) {
            lightEffects[name] = effect;
        }
    }

    LightEffect Canvas::getLightEffect(const std::string &name) {
        if (lightEffects.contains(name)) {
            return lightEffects[name];
        }

        throw std::runtime_error("Light effect not found");
    }

    void Canvas::setLightTintColor(const std::string &name, const Color &color) {
        auto it = lightEffects.find(name);
        if (it != lightEffects.end()) {
            it->second.tintColor = color;
        }
    }

    void Canvas::setLightOpacity(const std::string &name, float opacity) {
        auto it = lightEffects.find(name);
        if (it != lightEffects.end()) {
            it->second.opacity = opacity;
        }
    }

    void Canvas::applyLighting(const RenderTexture2D &baseTexture, const RenderTexture2D &renderTexture) {
        BeginTextureMode(renderTexture);
        BeginShaderMode(lightingShader);
        for (const auto &[name, effect] : lightEffects) {
            auto color = effect.tintColor;
            auto glColor = (Vector4){color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

            SetShaderValue(lightingShader, tintColorLoc, &glColor, SHADER_UNIFORM_VEC4);
            SetShaderValue(lightingShader, opacityLoc, &effect.opacity, SHADER_UNIFORM_FLOAT);

            SetShaderValueTexture(lightingShader, baseTextureLoc, baseTexture.texture);
            SetShaderValueTexture(lightingShader, maskTextureLoc, effect.maskTexture);

            DrawTextureEx(renderTexture.texture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        }

        // If there are no light effects, just draw the base texture
        if (lightEffects.empty()) {
            DrawTextureEx(baseTexture.texture, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
        }

        EndShaderMode();
        EndTextureMode();
    }
}
