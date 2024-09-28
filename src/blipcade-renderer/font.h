//
// Created by Pavlo Yevsehnieiev
//

#ifndef FONT_H
#define FONT_H
#include <canvas.h>
#include <unordered_map>

#include "spritesheet.h"
#include "graphics_types.h"

namespace blipcade::graphics {
class Font {
public:
    Font(Spritesheet spritesheet, std::unordered_map<wchar_t, uint32_t> glyphs, Size glyphSize);
    ~Font();

    static Font fromData(std::vector<uint8_t> data, Size sheetSize, Size glyphSize, std::wstring characters);

    [[nodiscard]] std::optional<Sprite> getGlyph(wchar_t c) const;
    [[nodiscard]] std::optional<uint32_t> getGlyphIndex(wchar_t c) const;
    [[nodiscard]] std::vector<uint32_t> getTextRun(const std::wstring &text) const;

    Spritesheet spritesheet;
    Size glyphSize;

private:
    std::unordered_map<wchar_t, uint32_t> glyphs;
};

} // graphics
// blipcade

#endif //FONT_H
