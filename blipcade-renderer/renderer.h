#pragma once
#include <cstdint>
#include "canvas.h"
#include "palette685.h"

namespace blipcade::graphics {
    class Renderer {
    public:
        Renderer(uint32_t width, uint32_t height, uint32_t scale);

        ~Renderer();

        void clear();

        void clear() const;

        void present(const Canvas &canvas) const;

        void setCanvas(const Canvas &canvas);

        void mainLoop();

        static void staticMainLoop();

        void createWindow();

    private:
        void render(const Canvas &canvas) const;

        void setupTexture();

        void updateWindowSize();

        uint32_t canvasWidth;
        uint32_t canvasHeight;
        uint32_t scale;
        uint32_t windowWidth;
        uint32_t windowHeight;

        const Palette685 *palette;
        const Canvas *canvas;

        static Renderer *instance;
    };
} // namespace blipcade::graphics
