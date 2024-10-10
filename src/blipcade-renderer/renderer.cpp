#include "renderer.h"

#include <audio.h>
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

    bool Renderer::handleInput(runtime::Runtime * const runtime) {

        if (IsKeyPressed(KEY_ESCAPE)) {
#ifndef EMSCRIPTEN
            if (devtool->isActive()) {
                devtool->setActive(false);
            } else {
                return true;
            }
#else
                CloseWindow();
#endif

        }

#ifndef EMSCRIPTEN
        if (devtool->isActive()) {
            return false;
        }
#else

#endif

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
        return false;
    }

    void Renderer::createWindow() {
#ifndef EMSCRIPTEN
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
#endif
        InitWindow(windowWidth, windowHeight, "Blipcade");
        InitAudioDevice();

        SetTargetFPS(30);
        SetExitKey(KEY_NULL);

        const auto runtime = new runtime::Runtime(canvasWidth, canvasHeight);
        setRuntime(*runtime);

        const RenderTexture2D runtimeRenderTexture = LoadRenderTexture(canvasWidth, canvasHeight);
        const RenderTexture2D postProcessTexture = LoadRenderTexture(canvasWidth, canvasHeight);
        const RenderTexture2D lightingTexture = LoadRenderTexture(canvasWidth, canvasHeight);

        BeginTextureMode(runtimeRenderTexture);
        runtime->init();
        EndTextureMode();

#ifndef EMSCRIPTEN
        HideCursor(); // For some unholy reason this cancels getting mouse position in emscripten
#endif

#ifndef EMSCRIPTEN
        rlImGuiSetup(true);
#endif

#ifndef EMSCRIPTEN
        devtool->init();
#endif
        while (!WindowShouldClose()) // Detect window close button or ESC key
        {
            if (handleInput(runtime)) break;

            // UpdateAudioStream(runtime->getAudio()->musicStream);

            const auto fps = GetFPS();

#ifndef EMSCRIPTEN
            devtool->setFPS(fps);
#endif


            runtime->update();

            int windowWidth = GetScreenWidth();
            int windowHeight = GetScreenHeight();

            BeginTextureMode(runtimeRenderTexture);


            runtime->draw(runtimeRenderTexture);

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
                static_cast<float>(runtimeRenderTexture.texture.width),
                static_cast<float>(runtimeRenderTexture.texture.height)
            };

            Vector2 origin = {0.0f, 0.0f};

            const float rotation = 0.0f;
#ifndef EMSCRIPTEN
            devtool->setCanvasOffset({offsetX, offsetY});
            devtool->setScale(scaleFactor);
            devtool->setCanvasSize({static_cast<float>(canvasWidth), static_cast<float>(canvasHeight)});
#endif


            // Mouse input
            const auto mouse_pos = GetMousePosition();

#ifndef EMSCRIPTEN
            if (!devtool->isActive()) {
#endif

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
#ifndef EMSCRIPTEN
            }
#endif


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

            runtime->postProcess(runtimeRenderTexture, postProcessTexture, srcRect, destRect, origin, rotation, {255, 255, 255, 255});

            DrawTexturePro(
                postProcessTexture.texture,
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
