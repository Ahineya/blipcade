#include <codecvt>
#include <iostream>

#include <renderer.h>
#include <quickjs.hpp>
#include <runtime.h>

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#else
#endif

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

    // try
    // {
    //     quickjs::runtime rt;
    //     quickjs::context ctx = rt.new_context();
    //
    //     quickjs::value global = ctx.get_global_object();
    //     global.set_property("test_func",
    //         [&](const std::string& val)
    //         {
    //             std::cout << "test_func: " << val << std::endl;
    //             return val;
    //         });
    //     global.set_property("test_func2",
    //         [&](const quickjs::args& a)
    //         {
    //             std::cout << "test_func2 with " << a.size() << " arg(s):" << std::endl;
    //             for (size_t i = 0; i < a.size(); i++)
    //                 std::cout << "    [" << i << "]: " << a[i].as_string() << std::endl;
    //         });
    //
    //     quickjs::value ret = ctx.eval(
    //         "test_func2(test_func(test_func.toString()), 3, 4.5);\n"
    //         "'done'");
    //     std::cout << "Value returned: " << (ret.valid() ? ret.as_cstring() : "[invalid]") << std::endl;
    //     // return 0;
    // }
    // catch (const quickjs::exception& e)
    // {
    //     std::cerr << "quickjs exception: " << e.what() << std::endl;
    //     // return 1;
    // }

    auto g_renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);
    auto runtime = new blipcade::runtime::Runtime();
    g_renderer->setRuntime(*runtime);
    g_renderer->createWindow();


    return 0;
}
