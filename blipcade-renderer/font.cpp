//
// Created by Pavlo Yevsehnieiev
//

#include "font.h"


namespace blipcade::graphics {
    Font::Font(const Spritesheet spritesheet, const std::unordered_map<wchar_t, uint32_t> glyphs,
               Size glyphSize): spritesheet(spritesheet), glyphs(glyphs), glyphSize(glyphSize) {
    }

    Font::~Font() {
        glyphs.clear();
    }

    Font Font::fromData(std::vector<uint8_t> data, Size sheetSize, Size glyphSize, std::wstring characters) {
        constexpr std::vector<uint8_t> emptySpriteData(0, 0);

        auto spritesheet = Spritesheet::fromData(data, emptySpriteData, sheetSize.width, sheetSize.height);
        std::unordered_map<wchar_t, uint32_t> glyphs;

        const auto textureWidth = spritesheet.width;

        const auto glyphWidth = glyphSize.width;
        const auto glyphHeight = glyphSize.height;

        auto x = 0;
        auto y = 0;

        for (const auto &c : characters) {
            const auto index = spritesheet.sprites.size();

            spritesheet.addSprite(x, y, glyphWidth, glyphHeight, 0);
            glyphs.insert({c, index});

            x += glyphWidth;
            if (x + glyphWidth > textureWidth) {
                x = 0;
                y += glyphHeight;
            }
        }

        return Font(spritesheet, glyphs, glyphSize);
    }

    std::optional<Sprite> Font::getGlyph(const wchar_t c) const {
        const auto index = glyphs.find(c);

        if (index != glyphs.end()) {
            return spritesheet.getSprite(index->second);
        }

        return std::nullopt;
    }

    std::optional<uint32_t> Font::getGlyphIndex(const wchar_t c) const {
        if (const auto index = glyphs.find(c); index != glyphs.end()) {
            return index->second;
        }

        return std::nullopt;
    }

    std::vector<uint32_t> Font::getTextRun(const std::wstring &text) const {
        std::vector<uint32_t> result;
        result.reserve(text.size());

        for (const auto &c : text) {
            if (const auto index = getGlyphIndex(c); index.has_value()) {
                result.push_back(index.value());
            }
        }

        return result;
    }
} // graphics
// blipcade
