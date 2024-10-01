//
// Created by Pavlo Yevsehnieiev
//

#ifndef RUNTIME_H
#define RUNTIME_H
#include <collision.h>
#include <memory>
#include <optional>
#include <raylib.h>
#include <string>

#include "keystate.h"
#include "mousestate.h"

#include "ECS.h"

namespace blipcade {
    namespace collision {
        class Collider;
    }

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

    class Runtime {
    public:
        Runtime(
            uint32_t width,
            uint32_t height
        );

        void evalWithStacktrace(const char *code) const;

        void keyDown(Key key);

        void keyUp(Key key);

        bool isKeyPressed(Key key) const;

        Vector2 getMousePos() const;

        bool isButtonPressed(MouseButton button) const;

        void mouseDown(MouseButton button);
        void mouseUp(MouseButton button);
        void mouseMove(int x, int y);

        void init();
        void update() const;
        void draw(const RenderTexture2D &renderTexture) const;

        void postProcess(const RenderTexture2D &postProcessTexture, const RenderTexture2D &renderTexture, const Rectangle &srcRect,
                         const Rectangle &destRect, const Vector2 &origin, float rotation, const Color &tint) const;

        [[nodiscard]] std::shared_ptr<quickjs::context> getContext() const;

        [[nodiscard]] std::shared_ptr<graphics::Canvas> getCanvas() const;

        [[nodiscard]] std::shared_ptr<graphics::Font> getFont() const;

        [[nodiscard]] std::shared_ptr<std::vector<graphics::Spritesheet>> getSpritesheets() const;

        std::shared_ptr<std::vector<collision::Collider>> getColliders() const;

        [[nodiscard]] std::shared_ptr<ecs::ECS> getECS() const;


        void setCartridge(std::shared_ptr<Cartridge>);

    private:
        std::unique_ptr<quickjs::runtime> js_runtime;
        std::shared_ptr<quickjs::context> context;

        std::shared_ptr<Cartridge> cartridge;

        std::shared_ptr<graphics::Canvas> canvas;
        std::shared_ptr<std::vector<graphics::Spritesheet>> spritesheets;
        std::shared_ptr<std::vector<collision::Collider>> colliders;
        std::shared_ptr<std::string> code;
        std::shared_ptr<Keystate> key_flags;
        std::shared_ptr<Mousestate> mouse_state;
        std::shared_ptr<graphics::Font> font;

        std::shared_ptr<ecs::ECS> ecs;

        std::unique_ptr<JSBindings> js_bindings;

        std::string cart_to_load;

        std::shared_ptr<std::optional<std::pair<uint32_t, uint32_t>>> resize_request;

        void register_global_functions();

        uint32_t canvasWidth;
        uint32_t canvasHeight;
    };

} // runtime
// blipcade

#endif //RUNTIME_H
