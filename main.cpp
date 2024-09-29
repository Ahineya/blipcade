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

static constexpr uint32_t WIDTH = 128;
static constexpr uint32_t HEIGHT = 128;

static constexpr uint32_t SCALE = 3;

int main() {
    const auto cart = std::make_shared<blipcade::Cartridge>(blipcade::Cartridge::fromJson(json_cart_data));
    const auto renderer = new blipcade::graphics::Renderer(WIDTH, HEIGHT, SCALE);
    const auto runtime = new blipcade::runtime::Runtime();
    runtime->setCartridge(cart);
    renderer->setRuntime(*runtime);
    renderer->createWindow();

    return 0;
}
