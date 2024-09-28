#include <codecvt>
#include <fstream>
#include <iostream>

#include <renderer.h>
#include <quickjs.hpp>
#include <runtime.h>

#include "cartridge.h"
#include "json_cart.hpp"

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#else
#endif

JSRuntime *rt;
JSContext *ctx;

bool running = false;

static constexpr uint32_t WIDTH = 128;
static constexpr uint32_t HEIGHT = 128;

static constexpr uint32_t SCALE = 3;

int main() {
    blipcade::Cartridge cart = blipcade::Cartridge::fromJson(json_cart_data);

    auto g_renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);
    auto runtime = new blipcade::runtime::Runtime();
    g_renderer->setRuntime(*runtime);
    g_renderer->createWindow();

    return 0;
}
