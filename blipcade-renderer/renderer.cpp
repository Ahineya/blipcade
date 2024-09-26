#include "renderer.h"
#include "palette685.h"
#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif

#include <iostream>
#include <GLFW/glfw3.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <GLES3/gl3.h>
#endif

GLFWwindow *window = nullptr;

GLuint shaderProgram;
GLuint VBO, VAO, EBO;
GLuint texture;

const char *vertexShaderSource =
#if defined(EMSCRIPTEN)
    "#version 300 es\n"
#else
        "#version 410\n"
#endif
        "layout (location = 0) in vec2 aPos;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\0";

const char *fragmentShaderSource =
#if defined(EMSCRIPTEN)
    "#version 300 es\n"
    "precision mediump float;\n"
#else
        "#version 410\n"
#endif
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "void main()\n"
    "{\n"
    "FragColor = texture(uTexture, TexCoord);\n"
    "}\n";

void checkShaderCompileErrors(GLuint shader, const char *type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void checkProgramLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog
                << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}

void compileShaders() {
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompileErrors(vertexShader, "VERTEX");

    // fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileErrors(fragmentShader, "FRAGMENT");

    // shader Program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinkErrors(shaderProgram);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void errorCallback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

namespace blipcade::graphics {

    Renderer* Renderer::instance = nullptr;

    Renderer::Renderer(const uint32_t width, const uint32_t height, const uint32_t scale): width(width), height(height),
        scale(scale),
        palette(nullptr),
        canvas(nullptr)
    {
        real_width = width * scale;
        real_height = height * scale;
        palette = new Palette685();
    }

    void Renderer::setCanvas(const Canvas &canvas) {
        this->canvas = &canvas;
    }

    void Renderer::mainLoop() {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        auto pixelData = canvas->getPixelsData();
        uint32_t* pixels = pixelData.data();

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE, pixels); // Ensure size matches
        glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Bind VAO and draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind VAO and texture
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Renderer::staticMainLoop() {
        if (instance) {
            instance->mainLoop();
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

        window = glfwCreateWindow(real_width, real_height, "Blipcade", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        compileShaders();

        setupTexture();

        // setupTriangle();

        std::cout << "Going into loop" << std::endl;
#ifdef EMSCRIPTEN
        emscripten_set_main_loop(Renderer::staticMainLoop, 0, 1);
#else
        while (!glfwWindowShouldClose(window)) {
            mainLoop();
        }
#endif

        std::cout << "Loop ended" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Renderer::initializeBuffers() {
    }

    void Renderer::setupTexture() {
        float vertices[] = {
            // Positions   // TexCoords
            -1.0f,  1.0f,   0.0f, 1.0f, // Top-left
            -1.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
             1.0f, -1.0f,   1.0f, 0.0f, // Bottom-right
             1.0f,  1.0f,   1.0f, 1.0f  // Top-right
        };

        unsigned int indices[] = {
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        // Create texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Allocate texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Renderer::~Renderer() {
        delete palette;
    }

    void Renderer::clear() {
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::present(const Canvas &canvas) const {
        // TODO: Implement OpenGL rendering of the canvas
        // For now, just swap buffers
        // SDL_GL_SwapWindow(window);
    }
} // namespace blipcade::graphics
