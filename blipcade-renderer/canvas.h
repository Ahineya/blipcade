#ifndef CANVAS_H
#define CANVAS_H
#include <cstdint>
#include <palette685.h>
#include <vector>

namespace blipcade::graphics {
    class Canvas {
    public:
        Canvas(uint32_t width, uint32_t height);

        ~Canvas();

        [[nodiscard]] const std::vector<uint8_t>& getPixels() const;

        std::vector<uint32_t> getPixelsData() const;

    private:
        std::array<uint32_t, 256> colorLookup{};
        std::vector<uint8_t> pixels;
        std::array<uint8_t, 256> virtualPalette{};
        std::unique_ptr<Palette685> palette;
    };
}


#endif //CANVAS_H
