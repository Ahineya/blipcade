// renderer.h
#ifndef BLIPCADE_GRAPHICS_CANVAS_HPP
#define BLIPCADE_GRAPHICS_CANVAS_HPP

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include <cstdint>

#include "palette685.h"

namespace blipcade {
    namespace graphics {

        class Renderer {
        public:
            Renderer(uint32_t width, uint32_t height);
            ~Renderer();

            void clear() const;
            void present() const;

        private:
            uint32_t width;
            uint32_t height;
            SDL_Window* window;
            SDL_Renderer* renderer;
            SDL_Texture* texture;
            Palette685* palette;
        };

    } // namespace graphics
} // namespace blipcade

#endif // BLIPCADE_GRAPHICS_CANVAS_HPP