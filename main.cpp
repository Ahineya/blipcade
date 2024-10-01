#include <renderer.h>
#include <quickjs.hpp>
#include <runtime.h>

#include "cartridge.h"
#include "json_cart_data.hpp"

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#else
#endif

JSRuntime *rt;
JSContext *ctx;

bool running = false;

static constexpr uint32_t WIDTH = 320;
static constexpr uint32_t HEIGHT = 240;

static constexpr uint32_t SCALE = 3;

int main() {
    const auto renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);

    renderer->createWindow();

    return 0;
}
