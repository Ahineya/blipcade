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

namespace blipcade::loader {
    class Project;
}

namespace blipcade {
    namespace audio {
        class Audio;
    }

    namespace collision {
        class Collider;
        class NavMesh;
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
    typedef struct {
        char *name;
        char *short_name;
        int flags;
    } namelist_entry_t;

    typedef struct namelist_t {
        namelist_entry_t *array;
        int count;
        int size;
    } namelist_t;

    static void find_unique_cname(char *cname, size_t cname_size);
    namelist_entry_t *namelist_find(namelist_t *lp, const char *name);
    JSModuleDef *jsc_module_loader(JSContext *ctx,
                                   const char *module_name, void *opaque);

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

        [[nodiscard]] std::shared_ptr<loader::Project> getProject() const;

        [[nodiscard]] std::shared_ptr<graphics::Canvas> getCanvas() const;

        [[nodiscard]] std::shared_ptr<graphics::Font> getFont() const;

        [[nodiscard]] std::shared_ptr<std::unordered_map<std::string, graphics::Spritesheet>> getSpritesheets() const;

        [[nodiscard]] std::shared_ptr<audio::Audio> getAudio() const;

        [[nodiscard]] std::shared_ptr<std::vector<collision::Collider>> getColliders() const;

        [[nodiscard]] std::shared_ptr<std::vector<collision::NavMesh>> getNavmeshes() const;

        [[nodiscard]] std::shared_ptr<ecs::ECS> getECS() const;

        void setCartridge(std::shared_ptr<Cartridge>);
        void setProject(std::shared_ptr<loader::Project>);

    private:
        std::unique_ptr<quickjs::runtime> js_runtime;
        std::shared_ptr<quickjs::context> context;

        std::shared_ptr<Cartridge> cartridge;

        std::shared_ptr<loader::Project> project;

        std::shared_ptr<graphics::Canvas> canvas;
        std::shared_ptr<std::unordered_map<std::string, graphics::Spritesheet>> spritesheets;
        std::shared_ptr<std::vector<collision::Collider>> colliders;
        std::shared_ptr<std::vector<collision::NavMesh>> navmeshes;
        std::shared_ptr<std::string> code;
        std::shared_ptr<Keystate> key_flags;
        std::shared_ptr<Mousestate> mouse_state;
        std::shared_ptr<graphics::Font> font;
        std::shared_ptr<audio::Audio> audio;

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
