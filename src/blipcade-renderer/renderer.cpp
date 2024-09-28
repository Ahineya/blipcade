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

#include <thread>

#include "shader_vert.h"
#include "shader_frag.h"

#include "canvas.h"
#include "runtime.h"

GLFWwindow *window = nullptr;

GLuint shaderProgram;
GLuint VBO, VAO, EBO;
GLuint texture;

void checkShaderCompileErrors(GLuint shader, const char *type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void checkProgramLinkErrors(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void compileShaders() {
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shader_vertexSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shader_fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompileErrors(fragmentShader, "FRAGMENT");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinkErrors(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

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
        glfwGetFramebufferSize(window, reinterpret_cast<int *>(&windowWidth), reinterpret_cast<int *>(&windowHeight));
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
            if (now - lastFPSTime >= 1.0) { // Every second
                // std::cout << "FPS: " << frameCount / (now - lastFPSTime) << std::endl;
                frameCount = 0;
                lastFPSTime = now;
            }

            glfwPollEvents();

            runtime->update();
            runtime->draw();
            updateWindowSize();

            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);

            // Calculate the aspect ratio of the window
            float windowAspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

            // Calculate the size of the square in normalized device coordinates
            float squareSize;
            if (windowAspectRatio > 1.0f) {
                squareSize = 1.0f;
            } else {
                squareSize = windowAspectRatio;
            }

            // Set uniforms
            GLint uSquareSizeLocation = glGetUniformLocation(shaderProgram, "uSquareSize");
            GLint uWindowAspectRatioLocation = glGetUniformLocation(shaderProgram, "uWindowAspectRatio");
            glUniform1f(uSquareSizeLocation, squareSize);
            glUniform1f(uWindowAspectRatioLocation, windowAspectRatio);

            auto c = runtime->getCanvas();
            const auto pixelData = c->getPixelsData();
            const auto *pixels = pixelData.data();

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvasWidth, canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glfwSwapBuffers(window);

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

        std::cout << "Starting" << std::endl;

        glfwSetErrorCallback(errorCallback);

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }

        #ifdef EMSCRIPTEN
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        #ifdef EMSCRIPTEN
        auto dpi = emscripten_get_device_pixel_ratio();
        uint32_t real_width = windowWidth * dpi;
        uint32_t real_height = windowHeight * dpi;
        window = glfwCreateWindow(windowWidth * dpi,  windowHeight * dpi, "Blipcade", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        #else
        window = glfwCreateWindow(windowWidth, windowHeight, "Blipcade", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        #endif

        #ifdef EMSCRIPTEN
        emscripten_set_element_css_size("canvas", windowWidth, windowHeight);
        #endif

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        glfwSetKeyCallback(window, staticKeyCallback);

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        compileShaders();
        setupTexture();

        runtime->init();

        std::cout << "Going into loop" << std::endl;
#ifdef EMSCRIPTEN
        emscripten_set_main_loop(Renderer::staticMainLoop, 30, 1);
#else
        while (!glfwWindowShouldClose(window)) {
            mainLoop();
        }
#endif

        std::cout << "Loop ended" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Renderer::setupTexture() const {
        const float vertices[] = {
            // Positions   // TexCoords
            -1.0f, 1.0f, 0.0f, 1.0f, // Top-left
            -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
            1.0f, 1.0f, 1.0f, 1.0f // Top-right
        };

        const unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Create texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Allocate texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvasWidth, canvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);

        // clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Renderer::~Renderer() {
        delete palette;
    }

    void Renderer::clear() {
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
    }
} // namespace blipcade::graphics
