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

    struct LightEffect {
        Color tintColor;        // The color tint to apply
        float opacity;          // Opacity of the tint (0.0 to 1.0)
        Texture2D maskTexture;  // Mask texture defining the shape/pattern
    };

    class Canvas {
    public:
        Canvas(uint32_t width, uint32_t height);

        ~Canvas();

        void setOffset(int32_t offsetX, int32_t offsetY);

        void setTransparentColor(uint8_t index);

        void fillScreen(uint8_t color);

        void setClipRect(Rect rect);

        bool isWithinClippingRect(int32_t x, int32_t y) const;

        void drawPixel(int32_t x, int32_t y, uint8_t color);

        void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color) const;

        void drawCircle(int32_t center_x, int32_t center_y, uint32_t radius, uint8_t color);

        void drawFilledCircle(int32_t center_x, int32_t center_y, uint32_t radius, uint8_t color);

        void drawRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);

        void drawRectangleW(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);

        void drawFilledRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t color);

        void drawFilledRectangleW(int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);

        void drawSprite(int32_t x, int32_t y, bool flipX, bool flipY,
                        const Spritesheet &spritesheet,
                        uint32_t index);

        void drawSpriteEx(int32_t x, int32_t y, bool flipX, bool flipY, float scale,
                          float originX, float originY, const Spritesheet &spritesheet, uint32_t index);

        void drawText(const Font &font, const std::wstring &text, int32_t x, int32_t y, std::optional<uint8_t> color);

        void clear();

        void setCamera(int32_t offsetX, int32_t offsetY);

        void addLightEffect(const std::string &name, const LightEffect &effect);
        void removeLightEffect(const std::string &name);
        void updateLightEffect(const std::string &name, const LightEffect &effect);

        LightEffect getLightEffect(const std::string &name);

        void setLightTintColor(const std::string &name, const Color &color);

        void setLightOpacity(const std::string &name, float opacity);

        void applyLighting(const RenderTexture2D &baseTexture, const RenderTexture2D &renderTexture);

    private:
        uint32_t width = 0;
        uint32_t height = 0;

        std::array<Color, 256> colorLookup;
        std::vector<uint8_t> pixels;
        std::array<uint8_t, 256> virtualPalette{};
        std::unique_ptr<Palette685> palette;

        int32_t offsetX = 0;
        int32_t offsetY = 0;

        uint8_t transparentColor = 0;

        Rect clipRect = {0, 0, 0, 0};

        Shader paletteShader;
        int paletteLoc;

        Texture2D paletteTexture;

        // Lighting overlay
        RenderTexture2D lightingRender;
        Shader lightingShader;

        int tintColorLoc;
        int opacityLoc;
        int baseTextureLoc;
        int maskTextureLoc;

        std::unordered_map<std::string, LightEffect> lightEffects;

        void updateShaderPalette();

        void createPaletteTexture();

        void updatePaletteTexture();

        void setPalette(const std::array<uint8_t, 256> &virtualPalette, const std::array<Color, 256> &colorLookup);
    };
}

#endif //CANVAS_H
