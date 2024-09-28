//
// Created by Pavlo Yevsehnieiev
//

#include "runtime.h"

#include <canvas.h>
#include <converters.h>
#include <iostream>
#include <quickjs.hpp>

#include "JsBindings.h"

// Header for reference:

/*
// Created by Pavlo Yevsehnieiev
//

#define RUNTIME_H
#include <memory>
#include <optional>
#include <string>


namespace blipcade::runtime {

 * So Runtime should be a singleton class that will be responsible for the main loop of the engine.
 *
 * It should know about:
 * - canvas
 * - quickjs runtime
 * - input
 * - audio


    // class JsRuntime;
    // class Context;
    class Canvas;
    class Palette685;
    class Spritesheet;
    // class Maps;
    // class KeyState;
    // class MouseState;
    class Font;
    // class RuntimeControl;
    // class AudioEngine;
    // class RuntimeCartList;

    class Runtime {
    public:
        Runtime(
            std::shared_ptr<Canvas> canvas,
            std::shared_ptr<Spritesheet> spritesheet,
            // std::shared_ptr<Maps> maps,
            // std::shared_ptr<KeyState> key_state,
            // std::shared_ptr<MouseState> mouse_state,
            // std::shared_ptr<RuntimeControl> runtime_state,
            std::shared_ptr<Font> font
            // #if !defined(__EMSCRIPTEN__)
            // std::shared_ptr<AudioEngine> audio_engine,
            // #else
            // std::shared_ptr<AudioEngine> audio_engine,
            // #endif
            // RuntimeCartList runtime_cart_list
        );

        void init();
        void update();
        void draw();

        // Let's make it possible to get a pointer to the canvas
        [[nodiscard]] std::shared_ptr<Canvas> getCanvas() const;

    private:
        // std::unique_ptr<JsRuntime> js_runtime;
        // std::shared_ptr<Context> context;
        std::shared_ptr<Canvas> canvas;
        std::shared_ptr<Spritesheet> spritesheet;
        // std::shared_ptr<Maps> maps;
        std::shared_ptr<std::string> code;
        // std::shared_ptr<KeyState> key_flags;
        // std::shared_ptr<MouseState> mouse_state;
        std::shared_ptr<Font> font;
        // std::shared_ptr<RuntimeControl> runtime_state;

// #if !defined(__EMSCRIPTEN__)
        // std::shared_ptr<AudioEngine> audio_engine;
// #else
        // std::shared_ptr<AudioEngine> audio_engine;
// #endif

        // RuntimeCartList runtime_cart_list;

        std::string cart_to_load;

        std::shared_ptr<std::optional<std::pair<uint32_t, uint32_t>>> resize_request;

        void register_global_functions();
    };

} // runtime
// blipcade

*/

namespace blipcade::runtime {
    Runtime::Runtime(): canvas(nullptr), font(nullptr), js_bindings(std::make_unique<JSBindings>(*this)) {
        canvas = std::make_shared<graphics::Canvas>(128, 128);
        std::cout << "Runtime constructed at " << this << " with Canvas at " << canvas.get()
                << " and pixels at " << canvas->getPixels().data() << std::endl;
        spritesheet = std::make_shared<graphics::Spritesheet>(0, 0);

        std::string fontHeader = "40 24 04 06";
        std::string fontData =
                "20 00 00 00 21 00 00 00 22 00 00 00 23 00 00 00 24 00 00 00 25 00 00 00 26 00 00 00 27 00 00 00 28 00 00 00 29 00 00 00 2A 00 00 00 2B 00 00 00 2C 00 00 00 2D 00 00 00 2E 00 00 00 2F 00 00 00 30 00 00 00 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00 39 00 00 00 3A 00 00 00 3B 00 00 00 3C 00 00 00 3D 00 00 00 3E 00 00 00 3F 00 00 00 40 00 00 00 41 00 00 00 42 00 00 00 43 00 00 00 44 00 00 00 45 00 00 00 46 00 00 00 47 00 00 00 48 00 00 00 49 00 00 00 4A 00 00 00 4B 00 00 00 4C 00 00 00 4D 00 00 00 4E 00 00 00 4F 00 00 00 50 00 00 00 51 00 00 00 52 00 00 00 53 00 00 00 54 00 00 00 55 00 00 00 56 00 00 00 57 00 00 00 58 00 00 00 59 00 00 00 5A 00 00 00 5B 00 00 00 5C 00 00 00 5D 00 00 00 5E 00 00 00 5F 00 00 00 60 00 00 00 61 00 00 00 62 00 00 00 63 00 00 00 64 00 00 00 65 00 00 00 66 00 00 00 67 00 00 00 68 00 00 00 69 00 00 00 6A 00 00 00 6B 00 00 00 6C 00 00 00 6D 00 00 00 6E 00 00 00 6F 00 00 00 70 00 00 00 71 00 00 00 72 00 00 00 73 00 00 00 74 00 00 00 75 00 00 00 76 00 00 00 77 00 00 00 78 00 00 00 79 00 00 00 7A 00 00 00 7B 00 00 00 7C 00 00 00 7D 00 00 00 7E 00 00 24 21";
        std::string fontImage =
                "FF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF FF FF FF FF EF FF EF EF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF FF FF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF FF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF EF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF EF EF FF EF FF EF FF FF EF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF EF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF EF FF FF FF FF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF FF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF FF FF EF FF FF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF EF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF FF FF EF EF EF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF FF FF EF EF FF FF FF EF FF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF FF FF EF FF EF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF";

        auto fontHeaderValues = api::split(fontHeader, ' ');
        auto fontHeaderBytes = api::convertToBytes(fontHeaderValues);
        auto fontDataValues = api::split(fontData, ' ');
        auto fontBytes = api::convertToBytes(fontDataValues);
        auto fontImageValues = api::split(fontImage, ' ');
        auto fontImageBytes = api::convertToBytes(fontImageValues);

        auto characters = api::decodeBeBytesToWstring(fontBytes);

        auto fontSpritesheetWidth = fontHeaderBytes[0];
        auto fontSpritesheetHeight = fontHeaderBytes[1];
        auto fontGlyphWidth = fontHeaderBytes[2];
        auto fontGlyphHeight = fontHeaderBytes[3];

        const auto font_obj = graphics::Font::fromData(fontImageBytes,
                                                       {fontSpritesheetWidth, fontSpritesheetHeight}, {
                                                           fontGlyphWidth, fontGlyphHeight
                                                       }, characters);

        font = std::make_shared<graphics::Font>(font_obj);
    }

    std::shared_ptr<graphics::Canvas> Runtime::getCanvas() const {
        return canvas;
    }

    std::shared_ptr<graphics::Font> Runtime::getFont() const {
        return font;
    }

    std::shared_ptr<quickjs::context> Runtime::getContext() const {
        return context;
    }


    void Runtime::init() {
        js_runtime = std::make_unique<quickjs::runtime>();
        context = std::make_shared<quickjs::context>(js_runtime->new_context());

        quickjs::value global = context->get_global_object();

        js_bindings->registerAll(global);

        auto code = R"javascript(
function init() {
    log(User.greet());
}
function update() {}
function draw() {
    //log("Hello from JavaScript!");
    //text("Hello from JavaScript!");
}
        )javascript";

        evalWithStacktrace(code);
        evalWithStacktrace("init()");
    }

    void Runtime::update() const {
        evalWithStacktrace("update()");
    }

    void Runtime::draw() const {
        evalWithStacktrace("draw()");
    }

    // TODO: I am not sure this works correctly in all the cases. Perhaps it would be better to handle it with C api directly.
    void Runtime::evalWithStacktrace(const char *code) const {
        try {
            quickjs::value ret = context->eval(
                code,
                "code.js"
            );
        } catch (const quickjs::value_error &e) {
            std::cerr << "JavaScript error: " << e.what() << std::endl;
            if (e.stack()) {
                std::cerr << "Stack trace: " << e.stack() << std::endl;
            }
        } catch (const quickjs::value_exception &e) {
            auto val = e.val();
            if (val.valid()) {
                std::cerr << "JavaScript exception: " << val.as_cstring().c_str() << std::endl;

                // Try to get the stack trace
                auto stack = val.get_property("stack");
                if (stack.valid() && !stack.is_undefined()) {
                    std::cerr << "Stack trace:\n" << stack.as_cstring().c_str() << std::endl;
                } else {
                    std::cerr << "No stack trace available." << std::endl;
                }
            } else {
                std::cerr << "Invalid JavaScript exception value" << std::endl;
            }
        } catch (const quickjs::value_error &e) {
            std::cerr << "JavaScript error: " << e.what() << std::endl;
            if (e.stack()) {
                std::cerr << "Stack trace:\n" << e.stack() << std::endl;
            }
        } catch (const quickjs::exception &e) {
            std::cerr << "QuickJS exception: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "C++ exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception" << std::endl;
        }
    }
} // runtime
// blipcade
