#include <codecvt>
#include <iostream>

#include "canvas.h"
#include "spritesheet.h"
#include "blipcade-api/converters.h"
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

    std::vector<uint8_t> pixels(3*3, 0xfe);
    pixels[8] = 0xFF;

    const std::vector<uint8_t> spriteData = {0, 0, 3, 3, 0};
    const auto spritesheet = blipcade::graphics::Spritesheet::fromData(pixels, spriteData, 3, 3);

    std::cout << spritesheet << std::endl;

    const auto data = spritesheet.getRectangleData(0, 0, 3, 3);

    g_canvas->drawSprite(0, 0, false, false, spritesheet, 0);

    std::string fontHeader = "40 24 04 06";
    std::string fontData = "20 00 00 00 21 00 00 00 22 00 00 00 23 00 00 00 24 00 00 00 25 00 00 00 26 00 00 00 27 00 00 00 28 00 00 00 29 00 00 00 2A 00 00 00 2B 00 00 00 2C 00 00 00 2D 00 00 00 2E 00 00 00 2F 00 00 00 30 00 00 00 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00 39 00 00 00 3A 00 00 00 3B 00 00 00 3C 00 00 00 3D 00 00 00 3E 00 00 00 3F 00 00 00 40 00 00 00 41 00 00 00 42 00 00 00 43 00 00 00 44 00 00 00 45 00 00 00 46 00 00 00 47 00 00 00 48 00 00 00 49 00 00 00 4A 00 00 00 4B 00 00 00 4C 00 00 00 4D 00 00 00 4E 00 00 00 4F 00 00 00 50 00 00 00 51 00 00 00 52 00 00 00 53 00 00 00 54 00 00 00 55 00 00 00 56 00 00 00 57 00 00 00 58 00 00 00 59 00 00 00 5A 00 00 00 5B 00 00 00 5C 00 00 00 5D 00 00 00 5E 00 00 00 5F 00 00 00 60 00 00 00 61 00 00 00 62 00 00 00 63 00 00 00 64 00 00 00 65 00 00 00 66 00 00 00 67 00 00 00 68 00 00 00 69 00 00 00 6A 00 00 00 6B 00 00 00 6C 00 00 00 6D 00 00 00 6E 00 00 00 6F 00 00 00 70 00 00 00 71 00 00 00 72 00 00 00 73 00 00 00 74 00 00 00 75 00 00 00 76 00 00 00 77 00 00 00 78 00 00 00 79 00 00 00 7A 00 00 00 7B 00 00 00 7C 00 00 00 7D 00 00 00 7E 00 00 24 21";
    std::string fontImage = "FF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF FF FF FF FF EF FF EF EF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF FF FF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF FF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF EF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF EF EF FF EF FF EF FF FF EF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF EF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF EF FF FF FF FF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF FF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF FF FF EF FF FF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF EF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF FF FF EF EF EF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF FF FF EF EF FF FF FF EF FF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF FF FF EF FF EF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF";

    auto fontHeaderValues = blipcade::api::split(fontHeader, ' ');
    auto fontHeaderBytes = blipcade::api::convertToBytes(fontHeaderValues);
    auto fontDataValues = blipcade::api::split(fontData, ' ');
    auto fontBytes = blipcade::api::convertToBytes(fontDataValues);
    auto fontImageValues = blipcade::api::split(fontImage, ' ');
    auto fontImageBytes = blipcade::api::convertToBytes(fontImageValues);

    auto characters = blipcade::api::decodeBeBytesToWstring(fontBytes);

    auto fontSpritesheetWidth = fontHeaderBytes[0];
    auto fontSpritesheetHeight = fontHeaderBytes[1];
    auto fontGlyphWidth = fontHeaderBytes[2];
    auto fontGlyphHeight = fontHeaderBytes[3];

    const auto font = blipcade::graphics::Font::fromData(fontImageBytes, {fontSpritesheetWidth, fontSpritesheetHeight}, {fontGlyphWidth, fontGlyphHeight}, characters);

    g_canvas->drawText(font, L"` !boobs", 70, 70, std::nullopt);
    g_canvas->drawText(font, L"` !boobs", 70, 80, 0x39);

    g_renderer->createWindow();

    return 0;
}
