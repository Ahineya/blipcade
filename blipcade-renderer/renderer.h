#pragma once
#include <cstdint>

namespace blipcade::runtime {
    class Runtime;
}

namespace blipcade::graphics {
    class Canvas;
    class Palette685;

    class Renderer {
    public:
        Renderer(uint32_t width, uint32_t height, uint32_t scale);

        ~Renderer();

        void clear();

        void clear() const;

        void present(const Canvas &canvas) const;

        void setCanvas(const Canvas &canvas);
        void setRuntime(runtime::Runtime &runtime);

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
        runtime::Runtime *runtime;

        static Renderer *instance;
    };
} // namespace blipcade::graphics
