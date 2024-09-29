#include "renderer.h"
#include "palette685.h"
#include <iostream>
#include <json_cart_data.hpp>

#include <raylib.h>
#include <thread>

#include "canvas.h"
#include "cartridge.h"
#include "runtime.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

void errorCallback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

namespace blipcade::graphics {
    Renderer *Renderer::instance = nullptr;

    Renderer::Renderer(const uint32_t width, const uint32_t height, const uint32_t scale)
        : canvasWidth(width), canvasHeight(height), scale(scale),
          windowWidth(width * scale), windowHeight(height * scale),
          palette(nullptr), canvas(nullptr), runtime(nullptr) {
        palette = new Palette685();
    }

    void Renderer::setCanvas(const Canvas &canvas) {
        this->canvas = &canvas;
    }

    void Renderer::setRuntime(runtime::Runtime &runtime) {
        this->runtime = &runtime;
    }

    void Renderer::staticKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::Left);
        } else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::Left);
        }

        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::Right);
        } else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::Right);
        }

        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::Up);
        } else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::Up);
        }

        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::Down);
        } else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::Down);
        }

        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::A);
        } else if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::A);
        }

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::A);
        } else if (key == GLFW_KEY_C && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::A);
        }

        if (key == GLFW_KEY_X && action == GLFW_PRESS) {
            instance->runtime->keyDown(runtime::Key::B);
        } else if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
            instance->runtime->keyUp(runtime::Key::B);
        }
    }

    void Renderer::createWindow() {
        instance = this;




#ifndef EMSCRIPTEN
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#endif
        InitWindow(windowWidth, windowHeight, "Blipcade");
        SetTargetFPS(30);

        const auto runtime = new runtime::Runtime();
        const auto cart = std::make_shared<Cartridge>(Cartridge::fromJson(json_cart_data));
        runtime->setCartridge(cart);
        setRuntime(*runtime);

        const RenderTexture2D renderTexture = LoadRenderTexture(canvasWidth, canvasHeight);

        BeginTextureMode(renderTexture);
        runtime->init();
        EndTextureMode();

        while (!WindowShouldClose()) // Detect window close button or ESC key
        {
            if (IsKeyDown(KEY_LEFT)) {
                runtime->keyDown(runtime::Key::Left);
            } else {
                runtime->keyUp(runtime::Key::Left);
            }

            if (IsKeyDown(KEY_RIGHT)) {
                runtime->keyDown(runtime::Key::Right);
            } else {
                runtime->keyUp(runtime::Key::Right);
            }

            if (IsKeyDown(KEY_UP)) {
                runtime->keyDown(runtime::Key::Up);
            } else {
                runtime->keyUp(runtime::Key::Up);
            }

            if (IsKeyDown(KEY_DOWN)) {
                runtime->keyDown(runtime::Key::Down);
            } else {
                runtime->keyUp(runtime::Key::Down);
            }

            if (IsKeyDown(KEY_Z)) {
                runtime->keyDown(runtime::Key::A);
            } else {
                runtime->keyUp(runtime::Key::A);
            }

            if (IsKeyDown(KEY_C)) {
                runtime->keyDown(runtime::Key::A);
            } else {
                runtime->keyUp(runtime::Key::A);
            }

            if (IsKeyDown(KEY_X)) {
                runtime->keyDown(runtime::Key::B);
            } else {
                runtime->keyUp(runtime::Key::B);
            }

            runtime->update();
            int windowWidth = GetScreenWidth();
            int windowHeight = GetScreenHeight();

            BeginTextureMode(renderTexture);

            runtime->draw();

            EndTextureMode();

            float canvasAspectRatio = static_cast<float>(canvasWidth) / static_cast<float>(canvasHeight);
            float windowAspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

            float scaleFactor;
            float scaledWidth, scaledHeight;
            float offsetX, offsetY;

            if (windowAspectRatio >= canvasAspectRatio) {
                scaleFactor = static_cast<float>(windowHeight) / static_cast<float>(canvasHeight);
                scaledWidth = canvasWidth * scaleFactor;
                scaledHeight = windowHeight;
                offsetX = (windowWidth - scaledWidth) / 2.0f;
                offsetY = 0.0f;
            } else {
                scaleFactor = static_cast<float>(windowWidth) / static_cast<float>(canvasWidth);
                scaledWidth = windowWidth;
                scaledHeight = canvasHeight * scaleFactor;
                offsetX = 0.0f;
                offsetY = (windowHeight - scaledHeight) / 2.0f;
            }

            Rectangle destRect = {
                offsetX,
                offsetY,
                scaledWidth,
                scaledHeight
            };

            Rectangle srcRect = {
                0.0f,
                0.0f,
                static_cast<float>(renderTexture.texture.width),
                static_cast<float>(-renderTexture.texture.height)
            };

            Vector2 origin = {0.0f, 0.0f};

            float rotation = 0.0f;

            BeginDrawing();

            DrawTexturePro(
                renderTexture.texture,
                srcRect,
                destRect,
                origin,
                rotation,
                {255, 255, 255, 255}
            );

            EndDrawing();
        }

        CloseWindow();
    }

    void Renderer::setupTexture() const {
    }

    Renderer::~Renderer() {
        delete palette;
    }

    void Renderer::clear() {
    }
} // namespace blipcade::graphics
