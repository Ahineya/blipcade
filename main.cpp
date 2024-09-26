#include <iostream>

#include "canvas.h"
#include "blipcade-renderer/renderer.h"
#include "external/SDL2/include/SDL_events.h"
extern "C" {
#include "quickjs.h"
}

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <SDL2/SDL.h>
#else
    #include <SDL.h>
#endif

blipcade::graphics::Renderer* g_renderer = nullptr;
blipcade::graphics::Canvas* g_canvas = nullptr;

JSRuntime *rt;
JSContext *ctx;

bool running = false;

static uint32_t const WIDTH = 128;
static uint32_t const HEIGHT = 128;

static uint32_t const SCALE = 3;

static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (str) {
            printf("%s\n", str);
            JS_FreeCString(ctx, str);
        }
    }
    return JS_UNDEFINED;
}

void initJS() {
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));
    JS_SetPropertyStr(ctx, global_obj, "console", console);
    JS_FreeValue(ctx, global_obj);

    const char *script = "console.log('Hello from QuickJS!');";
    JSValue val = JS_Eval(ctx, script, strlen(script), "<input>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(val)) {
        JSValue exc = JS_GetException(ctx);
        const char *str = JS_ToCString(ctx, exc);
        printf("Exception: %s\n", str);
        JS_FreeCString(ctx, str);
        JS_FreeValue(ctx, exc);
    }
    JS_FreeValue(ctx, val);
}

void cleanupJS() {
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

void mainLoop() {
    SDL_Event event;

    // here we want to:
    // 1. Cap the FPS
    // 2. Run the update function
    // 3. Run the render function
    // 4. Handle input
    // 5. Handle events
    // 6. Rinse and repeat


    g_renderer->clear();
    g_renderer->present(*g_canvas);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        // Handle input
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    // Move up
                    break;
                case SDLK_DOWN:
                    // Move down
                    break;
                case SDLK_LEFT:
                    // Move left
                    break;
                case SDLK_RIGHT:
                    // Move right
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:
                    break;
            }
        }
    }
}

int main() {
    try {
        g_renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);
        g_canvas = new blipcade::graphics::Canvas(WIDTH, HEIGHT);
        running = true;

        initJS();

#if __EMSCRIPTEN__
        emscripten_set_main_loop(mainLoop, -1, 1);
#else
        while (running) {
            mainLoop();
        }
#endif

        cleanupJS();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    delete g_renderer;
    return 0;
}