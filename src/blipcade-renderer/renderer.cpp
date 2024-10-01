#include "renderer.h"

#include <imgui.h>

#include "palette685.h"
#include <iostream>

#include <raylib.h>
#include <thread>

#include "canvas.h"
#include "mousestate.h"
#include "runtime.h"
#include "rlImGui.h"
#include "devtool.h"

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
        devtool = std::make_unique<devtool::Devtool>(runtime);
    }

    void Renderer::createWindow() {
#ifndef EMSCRIPTEN
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#endif
        InitWindow(windowWidth, windowHeight, "Blipcade");
        InitAudioDevice();

        auto sound = LoadSound("resources/blipcade.wav");

        SetTargetFPS(30);
        SetExitKey(KEY_NULL);

        const auto runtime = new runtime::Runtime(canvasWidth, canvasHeight);
        setRuntime(*runtime);

        const RenderTexture2D renderTexture = LoadRenderTexture(canvasWidth, canvasHeight);
        const RenderTexture2D postProcessTexture = LoadRenderTexture(canvasWidth, canvasHeight);
        const RenderTexture2D lightingTexture = LoadRenderTexture(canvasWidth, canvasHeight);

        BeginTextureMode(renderTexture);
        runtime->init();
        EndTextureMode();

#ifndef EMSCRIPTEN
        HideCursor(); // For some unholy reason this cancels getting mouse position in emscripten
#endif

#ifndef EMSCRIPTEN
        rlImGuiSetup(true);
#endif


        while (!WindowShouldClose()) // Detect window close button or ESC key
        {

            if (IsKeyPressed(KEY_ESCAPE)) {
#ifndef EMSCRIPTEN
                if (devtool->isActive()) {
                    devtool->setActive(false);
                } else {
                    break;
                }
#else
                CloseWindow();
#endif

            }

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

            // ctrl/cmd + alt + i to open dev tools
            if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER)) && IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_I)) {
                devtool->setActive(!devtool->isActive());
            }

            const auto fps = GetFPS();
            devtool->setFPS(fps);

            runtime->update();
            int windowWidth = GetScreenWidth();
            int windowHeight = GetScreenHeight();

            BeginTextureMode(renderTexture);

            runtime->draw(renderTexture);

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

            const float rotation = 0.0f;

            devtool->setCanvasOffset({offsetX, offsetY});
            devtool->setScale(scaleFactor);

            // Mouse input
            const auto mouse_pos = GetMousePosition();

            // Need to convert to canvas coordinates and account for position of canvas
            runtime->mouseMove((mouse_pos.x - offsetX) / scaleFactor, (mouse_pos.y - offsetY) / scaleFactor); // TODO: Check if this is correct

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                runtime->mouseDown(runtime::MouseButton::Left);
            } else {
                runtime->mouseUp(runtime::MouseButton::Left);
            }

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                runtime->mouseDown(runtime::MouseButton::Right);
            } else {
                runtime->mouseUp(runtime::MouseButton::Right);
            }

            BeginDrawing();
#ifndef EMSCRIPTEN
            rlImGuiBegin();
#endif

#ifndef EMSCRIPTEN
            if (devtool->isActive()) {
                devtool->draw();
            }
#endif

            // clear
            ClearBackground(BLACK);

            // We want to render this BS to another texture so we can apply post processing
            BeginTextureMode(postProcessTexture);
                DrawTexture(
                    renderTexture.texture,
                    0,
                    0,
                    WHITE
                );
            EndTextureMode();

            runtime->postProcess(postProcessTexture, lightingTexture, srcRect, destRect, origin, rotation, {255, 255, 255, 255});

            DrawTexturePro(
                lightingTexture.texture,
                srcRect,
                destRect,
                origin,
                rotation,
                {255, 255, 255, 255}
            );

#ifndef EMSCRIPTEN
            rlImGuiEnd();
#endif

            EndDrawing();
        }

#ifndef EMSCRIPTEN
        rlImGuiShutdown();
#endif

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
