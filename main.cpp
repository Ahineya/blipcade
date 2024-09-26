#include <iostream>

#include "canvas.h"
#include "blipcade-renderer/renderer.h"

extern "C" {
#include "quickjs.h"
}

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#else
#endif

blipcade::graphics::Renderer *g_renderer = nullptr;
blipcade::graphics::Canvas *g_canvas = nullptr;

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

int main() {
    initJS();
    g_renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);
    g_canvas = new blipcade::graphics::Canvas(WIDTH, HEIGHT);

    g_renderer->setCanvas(*g_canvas);

    g_renderer->createWindow();

    return 0;
}
