// renderer.cpp
#include "renderer.h"
#include "palette685.h"
#include "color.h"
#include <stdexcept>
#include <string>

namespace blipcade::graphics {
    Renderer::Renderer(const uint32_t width, const uint32_t height): width(width), height(height), window(nullptr),
                                                                     renderer(nullptr), texture(nullptr),
                                                                     palette(nullptr) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));
        }

        window = SDL_CreateWindow(
            "Blipcade",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN
        );

        if (window == nullptr) {
            SDL_Quit();
            throw std::runtime_error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
                                                  SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

        palette = new Palette685();
    }

    Renderer::~Renderer() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_DestroyTexture(texture);
        delete palette;
        SDL_Quit();
    }

    void Renderer::clear() const {
        auto color = Color::from_hex(0x000000FF);

        SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    void Renderer::present() const {
        void *pixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &pixels, &pitch);

        const auto color = palette->get_color(0x50);

        for (int y = 20; y < height - 20; y++) {
            for (int x = 20; x < width - 20; x++) {
                auto *pixel = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(pixels) + y * pitch + x * 4);
                *pixel = color.to_hex();
            }
        }

        SDL_UnlockTexture(texture);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
} // namespace blipcade::graphics
