#include "renderer.h"
#include "palette685.h"
#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif

#include <GLFW/glfw3.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#endif

#include <raylib.h>
#include <thread>

#include "shader_vert.h"
#include "shader_frag.h"

#include "canvas.h"
#include "runtime.h"

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

    void Renderer::updateWindowSize() {
        // glfwGetFramebufferSize(window, reinterpret_cast<int *>(&windowWidth), reinterpret_cast<int *>(&windowHeight));
    }

    void Renderer::setCanvas(const Canvas &canvas) {
        this->canvas = &canvas;
    }

    void Renderer::setRuntime(runtime::Runtime &runtime) {
        this->runtime = &runtime;
    }

    static const double fpsLimit = 1.0 / 30.0;
    static double lastUpdateTime = 0;
    static double lastFrameTime = 0;

    static double lastFPSTime = 0;
    static int frameCount = 0;

    void Renderer::mainLoop() {
        static bool first_frame = true;
        if (first_frame) {
            first_frame = false;
        }

        const double now = glfwGetTime();

        // In case we're running in the browser, we don't want to limit the frame rate here — it is handled by emscripten_set_main_loop
        // Still need to check everything, I feel it may not work as intended
#ifndef EMSCRIPTEN
        if (now - lastFrameTime >= fpsLimit) {
#endif

            frameCount++;
            if (now - lastFPSTime >= 1.0) {
                // Every second
                // std::cout << "FPS: " << frameCount / (now - lastFPSTime) << std::endl;
                frameCount = 0;
                lastFPSTime = now;
            }


            runtime->update();
            runtime->draw();
            updateWindowSize();

            glClear(GL_COLOR_BUFFER_BIT);


            lastFrameTime = now;
            lastUpdateTime = now;
#ifndef EMSCRIPTEN
        } else {
            double sleepTime = fpsLimit - (now - lastFrameTime);
            glfwWaitEventsTimeout(sleepTime / 2.0); // Dividing by two here to get as close to 30 fps as possible
        }
#endif
    }

    void Renderer::staticMainLoop() {
        if (instance) {
            instance->mainLoop();
        }
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

        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(windowWidth, windowHeight, "Blipcade");
        SetTargetFPS(30);


#ifdef EMSCRIPTEN
        emscripten_set_element_css_size("canvas", windowWidth, windowHeight);
#endif

        RenderTexture2D renderTexture = LoadRenderTexture(canvasWidth, canvasHeight);

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

            // --- Calculate Scaling and Positioning ---
            // Calculate aspect ratios
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
                // Window is taller relative to canvas aspect ratio
                scaleFactor = static_cast<float>(windowWidth) / static_cast<float>(canvasWidth);
                scaledWidth = windowWidth;
                scaledHeight = canvasHeight * scaleFactor;
                offsetX = 0.0f;
                offsetY = (windowHeight - scaledHeight) / 2.0f;
            }

            // Define the destination rectangle with scaling and centering
            Rectangle destRect = {
                offsetX, // X position
                offsetY, // Y position
                scaledWidth, // Width after scaling
                scaledHeight // Height after scaling
            };

            // Define the source rectangle, flipping vertically as needed
            // Raylib's render textures are flipped vertically by default
            Rectangle srcRect = {
                0.0f,
                (float) renderTexture.texture.height, // Start from bottom
                (float) renderTexture.texture.width,
                (float) -renderTexture.texture.height // Negative height to flip
            };

            // Define the origin (top-left corner)
            Vector2 origin = {0.0f, 0.0f};

            // Rotation angle (0 for no rotation)
            float rotation = 0.0f;

            // Color tint (WHITE to keep original colors)
            auto tint = {255, 255, 255, 255};

            // --- Draw to Screen ---
            BeginDrawing();
            // ClearBackground(BLACK); // Optional: clear the screen with a background color

            // Draw the render texture to the screen with scaling and centering
            DrawTexturePro(
                renderTexture.texture, // Texture to draw
                srcRect, // Source rectangle (what part of the texture to draw)
                destRect, // Destination rectangle (where and how to draw it on screen)
                origin, // Origin point for rotation and scaling
                rotation, // Rotation angle
                {255, 255, 255, 255} // Tint color
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
