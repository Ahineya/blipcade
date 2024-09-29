//
// Created by Pavlo Yevsehnieiev
//

#ifndef RUNTIME_H
#define RUNTIME_H
#include <memory>
#include <optional>
#include <string>

#include "keystate.h"

namespace blipcade {
    class Cartridge;
}

namespace quickjs {
    class context;
}

namespace quickjs {
    class runtime;
}

namespace blipcade::graphics {
    class Font;
    class Canvas;
    class Palette685;
    class Spritesheet;
}

namespace blipcade::runtime {
    class JSBindings;
    class Keystate;

    /*
 * So Runtime should be a singleton class that will be responsible for the main loop of the engine.
 *
 * It should know about:
 * - canvas
 * - quickjs runtime
 * - input
 * - audio
 */

    // class JsRuntime;
    // class Context;
    // class Maps;
    // class KeyState;
    // class MouseState;
    // class RuntimeControl;
    // class AudioEngine;
    // class RuntimeCartList;

    class Runtime {
    public:
        Runtime(
            // std::shared_ptr<graphics::Canvas> canvas,
            // std::shared_ptr<graphics::Spritesheet> spritesheet,
            // std::shared_ptr<Maps> maps,
            // std::shared_ptr<KeyState> key_state,
            // std::shared_ptr<MouseState> mouse_state,
            // std::shared_ptr<RuntimeControl> runtime_state,
            // std::shared_ptr<graphics::Font> font
            // #if !defined(__EMSCRIPTEN__)
            // std::shared_ptr<AudioEngine> audio_engine,
            // #else
            // std::shared_ptr<AudioEngine> audio_engine,
            // #endif
            // RuntimeCartList runtime_cart_list
        );

        void evalWithStacktrace(const char *code) const;

        void keyDown(Key key);

        void keyUp(Key key);

        bool isKeyPressed(Key key) const;

        void init();
        void update() const;
        void draw() const;

        [[nodiscard]] std::shared_ptr<quickjs::context> getContext() const;

        [[nodiscard]] std::shared_ptr<graphics::Canvas> getCanvas() const;

        [[nodiscard]] std::shared_ptr<graphics::Font> getFont() const;

        void setCartridge(std::shared_ptr<Cartridge>);

    private:
        std::unique_ptr<quickjs::runtime> js_runtime;
        std::shared_ptr<quickjs::context> context;

        std::shared_ptr<Cartridge> cartridge;

        std::shared_ptr<graphics::Canvas> canvas;
        std::shared_ptr<graphics::Spritesheet> spritesheet;
        // std::shared_ptr<Maps> maps;
        std::shared_ptr<std::string> code;
        std::shared_ptr<Keystate> key_flags;
        // std::shared_ptr<MouseState> mouse_state;
        std::shared_ptr<graphics::Font> font;

        std::unique_ptr<JSBindings> js_bindings;
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

#endif //RUNTIME_H
