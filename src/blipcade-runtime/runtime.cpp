//
// Created by Pavlo Yevsehnieiev
//

#include "runtime.h"

#include <canvas.h>
#include <converters.h>
#include <postprocessing.h>
#include <project.h>

extern "C" {
    #include <cutils.h>
}
#include <iostream>
#include <json_cart_data.hpp>
#include <quickjs-libc.h>
#include <nlohmann/json.hpp>
#include <quickjs.hpp>

#include "cartridge.h"
#include "JsBindings.h"
#include "keystate.h"
#include "collider.h"
#include "audio.h"

auto const WIDTH = 320;
auto const HEIGHT = 240;

namespace blipcade::runtime {
    Runtime::Runtime(uint32_t width, uint32_t height): cartridge(nullptr), canvas(nullptr),
                                                       key_flags(std::make_shared<Keystate>()),
                                                       mouse_state(std::make_shared<Mousestate>()), font(nullptr),
                                                       js_bindings(std::make_unique<JSBindings>(*this)),
                                                       canvasWidth(width), canvasHeight(height), audio(nullptr),
                                                       navmeshes(nullptr) {
        canvas = std::make_shared<graphics::Canvas>(canvasWidth, canvasHeight);
        spritesheets = std::make_shared<std::unordered_map<std::string, graphics::Spritesheet> >();
        colliders = std::make_shared<std::unordered_map<std::string, collision::Collider> >();
        navmeshes = std::make_shared<std::unordered_map<std::string, collision::NavMesh> >();
        audio = std::make_shared<audio::Audio>();
        postprocessing = std::make_shared<renderer::Postprocessing>();

        // std::string fontHeader = "40 24 04 06";
        // std::string fontData =
        // "20 00 00 00 21 00 00 00 22 00 00 00 23 00 00 00 24 00 00 00 25 00 00 00 26 00 00 00 27 00 00 00 28 00 00 00 29 00 00 00 2A 00 00 00 2B 00 00 00 2C 00 00 00 2D 00 00 00 2E 00 00 00 2F 00 00 00 30 00 00 00 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00 39 00 00 00 3A 00 00 00 3B 00 00 00 3C 00 00 00 3D 00 00 00 3E 00 00 00 3F 00 00 00 40 00 00 00 41 00 00 00 42 00 00 00 43 00 00 00 44 00 00 00 45 00 00 00 46 00 00 00 47 00 00 00 48 00 00 00 49 00 00 00 4A 00 00 00 4B 00 00 00 4C 00 00 00 4D 00 00 00 4E 00 00 00 4F 00 00 00 50 00 00 00 51 00 00 00 52 00 00 00 53 00 00 00 54 00 00 00 55 00 00 00 56 00 00 00 57 00 00 00 58 00 00 00 59 00 00 00 5A 00 00 00 5B 00 00 00 5C 00 00 00 5D 00 00 00 5E 00 00 00 5F 00 00 00 60 00 00 00 61 00 00 00 62 00 00 00 63 00 00 00 64 00 00 00 65 00 00 00 66 00 00 00 67 00 00 00 68 00 00 00 69 00 00 00 6A 00 00 00 6B 00 00 00 6C 00 00 00 6D 00 00 00 6E 00 00 00 6F 00 00 00 70 00 00 00 71 00 00 00 72 00 00 00 73 00 00 00 74 00 00 00 75 00 00 00 76 00 00 00 77 00 00 00 78 00 00 00 79 00 00 00 7A 00 00 00 7B 00 00 00 7C 00 00 00 7D 00 00 00 7E 00 00 24 21";
        // std::string fontImage =
        // "FF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF FF FF FF FF EF FF EF EF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF FF FF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF FF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF EF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF EF EF FF EF FF EF FF FF EF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF EF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF EF FF FF FF FF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF FF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF FF FF EF FF FF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF EF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF FF FF EF EF EF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF FF FF EF EF FF FF FF EF FF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF FF FF EF FF EF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF";

        std::string fontHeader = "60 24 05 07";
        std::string fontData =
                "20 00 00 00 21 00 00 00 22 00 00 00 23 00 00 00 24 00 00 00 25 00 00 00 26 00 00 00 27 00 00 00 28 00 00 00 29 00 00 00 2A 00 00 00 2B 00 00 00 2C 00 00 00 2D 00 00 00 2E 00 00 00 2F 00 00 00 30 00 00 00 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00 39 00 00 00 3A 00 00 00 3B 00 00 00 3C 00 00 00 3D 00 00 00 3E 00 00 00 3F 00 00 00 40 00 00 00 41 00 00 00 42 00 00 00 43 00 00 00 44 00 00 00 45 00 00 00 46 00 00 00 47 00 00 00 48 00 00 00 49 00 00 00 4A 00 00 00 4B 00 00 00 4C 00 00 00 4D 00 00 00 4E 00 00 00 4F 00 00 00 50 00 00 00 51 00 00 00 52 00 00 00 53 00 00 00 54 00 00 00 55 00 00 00 56 00 00 00 57 00 00 00 58 00 00 00 59 00 00 00 5A 00 00 00 5B 00 00 00 5C 00 00 00 5D 00 00 00 5E 00 00 00 5F 00 00 00 60 00 00 00 61 00 00 00 62 00 00 00 63 00 00 00 64 00 00 00 65 00 00 00 66 00 00 00 67 00 00 00 68 00 00 00 69 00 00 00 6A 00 00 00 6B 00 00 00 6C 00 00 00 6D 00 00 00 6E 00 00 00 6F 00 00 00 70 00 00 00 71 00 00 00 72 00 00 00 73 00 00 00 74 00 00 00 75 00 00 00 76 00 00 00 77 00 00 00 78 00 00 00 79 00 00 00 7A 00 00 00 7B 00 00 00 7C 00 00 00 7D 00 00 00 7E 00 00 24 21";

        std::string fontImage =
                "FF FF FF FF FF EF FF FF FF FF FF EF FF EF FF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF FF EF EF FF FF FF EF FF FF FF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF EF EF EF FF FF FF EF FF FF EF EF EF EF FF FF FF FF FF FF FF EF FF FF FF FF EF FF FF EF FF EF EF EF EF FF FF EF EF EF FF FF FF FF EF FF EF FF FF EF FF EF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF FF EF FF FF EF EF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF FF EF EF FF FF EF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF FF FF EF FF FF FF EF FF FF EF EF EF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF EF FF FF EF EF EF FF FF EF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF EF FF FF EF FF FF EF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF EF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF EF EF FF FF FF FF FF FF FF FF EF EF FF FF FF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF FF FF EF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF EF FF FF FF FF EF FF FF FF EF EF EF EF FF FF FF EF FF FF EF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF EF FF EF FF FF EF FF EF EF EF EF FF EF EF EF EF FF EF EF EF EF FF EF EF EF EF FF EF EF EF EF FF EF EF FF FF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF FF EF EF FF FF FF EF EF FF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF EF EF EF FF FF FF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF FF FF FF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF EF FF FF FF EF EF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF EF EF EF FF EF EF EF EF FF EF EF EF EF FF EF EF EF EF FF FF FF FF EF FF EF EF EF EF FF EF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF EF EF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF EF FF EF EF EF EF FF EF EF EF FF FF EF FF FF FF FF EF FF FF EF FF EF EF EF FF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF FF EF FF FF FF FF EF FF EF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF FF EF FF FF FF FF EF FF EF FF FF EF FF FF FF FF EF FF EF EF FF FF FF EF EF FF FF FF FF FF EF FF FF EF EF EF FF FF FF FF EF FF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF FF EF EF EF EF FF FF FF FF EF FF EF EF EF EF FF EF EF EF EF FF FF FF FF EF FF EF EF EF EF FF EF EF EF EF FF EF EF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF EF FF FF EF FF FF EF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF FF EF EF EF FF EF FF FF EF FF EF EF EF FF FF FF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF EF EF FF EF FF FF EF FF FF EF EF FF FF EF EF EF FF FF FF EF EF FF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF FF FF FF FF FF FF EF FF EF FF EF FF FF EF FF FF FF FF EF EF EF EF FF EF EF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF EF FF FF FF EF EF EF FF FF EF FF FF FF FF EF EF EF EF FF FF EF FF FF FF FF FF FF EF FF EF EF FF FF FF EF FF FF FF FF EF EF FF EF FF EF EF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF EF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF FF EF FF FF FF FF EF FF FF FF FF EF FF EF EF FF EF FF FF EF FF FF EF FF FF FF FF FF FF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF EF FF EF FF EF EF FF EF FF FF EF FF EF EF EF FF FF EF FF FF EF FF EF EF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF EF FF EF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF EF FF EF FF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF EF FF EF FF EF EF FF EF FF FF EF FF EF FF FF FF FF EF FF EF FF FF EF FF FF EF FF FF FF FF EF FF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF EF FF EF FF EF FF FF FF EF FF FF FF FF FF EF EF EF FF EF FF FF EF FF EF EF EF FF FF FF EF EF FF FF EF FF FF EF FF EF EF EF EF FF EF FF FF EF FF EF FF FF EF FF FF EF EF FF FF EF FF FF FF FF FF EF FF EF FF EF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF EF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF FF EF EF EF EF FF FF FF EF EF FF EF FF FF FF FF FF EF EF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF FF FF FF FF EF FF FF FF EF FF FF EF FF FF FF FF FF FF EF FF FF FF EF FF EF FF FF FF FF FF FF EF EF FF FF FF FF EF EF FF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF EF EF EF FF EF EF EF EF FF FF EF EF EF FF EF FF FF EF FF EF EF EF FF FF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF FF FF FF FF FF FF EF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF EF FF EF EF EF FF FF EF FF FF FF FF EF FF FF EF FF EF EF EF FF FF EF EF EF FF FF EF FF EF EF FF EF EF EF EF FF FF EF FF FF FF FF FF FF EF FF EF EF FF FF FF FF FF EF FF FF FF EF FF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF EF FF EF FF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF FF EF FF FF FF EF EF EF EF FF FF FF EF EF FF FF FF EF FF FF FF EF EF FF FF FF FF FF FF FF EF EF EF EF FF FF FF FF FF FF EF FF FF EF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF EF EF EF FF EF FF FF FF FF FF EF EF FF FF EF FF FF EF FF EF EF EF FF FF FF EF EF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF FF EF FF FF FF EF EF FF FF FF FF FF FF FF FF EF FF FF FF FF EF FF EF EF FF EF EF EF FF FF FF EF EF FF FF EF EF EF FF FF FF EF EF FF FF EF EF EF FF FF FF EF EF EF FF EF EF EF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF EF FF FF EF FF EF FF FF EF EF EF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF FF FF EF EF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF EF FF FF EF FF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF EF EF FF FF EF FF FF EF FF EF EF EF FF FF FF EF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF EF FF EF EF FF EF FF FF EF FF FF FF FF EF FF FF FF FF EF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF EF FF EF FF FF EF FF EF FF FF EF FF EF FF FF FF FF EF FF EF FF FF EF FF FF EF FF FF FF FF EF FF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF EF FF EF FF EF FF FF FF EF FF FF FF EF FF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF EF EF EF EF FF EF FF FF EF FF EF FF FF EF FF FF EF EF FF FF EF FF FF FF FF FF EF FF EF FF EF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF EF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF EF FF FF FF EF EF EF EF FF FF FF EF EF FF FF FF EF FF FF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF";

        auto fontHeaderValues = api::split(fontHeader, ' ');
        auto fontHeaderBytes = api::convertToBytes(fontHeaderValues);
        auto fontDataValues = api::split(fontData, ' ');
        auto fontBytes = api::convertToBytes(fontDataValues);
        auto fontImageValues = api::split(fontImage, ' ');
        auto fontImageBytes = api::convertToBytes(fontImageValues);

        auto characters = api::decodeBeBytesToWstring(fontBytes);

        auto fontSpritesheetWidth = fontHeaderBytes[0];
        auto fontSpritesheetHeight = fontHeaderBytes[1];
        auto fontGlyphWidth = fontHeaderBytes[2];
        auto fontGlyphHeight = fontHeaderBytes[3];

        const auto font_obj = graphics::Font::fromData(fontImageBytes,
                                                       {fontSpritesheetWidth, fontSpritesheetHeight}, {
                                                           fontGlyphWidth, fontGlyphHeight
                                                       }, characters);

        font = std::make_shared<graphics::Font>(font_obj);

        const auto cart = std::make_shared<Cartridge>(Cartridge::fromJson(json_cart_data));
        cartridge = cart;

        const auto project = std::make_shared<loader::Project>("../src/projects/echoes-of-her");
        setProject(project);

        const auto spritesheets = cart->getSpritesheets();
        for (const auto &spritesheet: spritesheets) {
            // this->spritesheets->push_back(spritesheet);
            this->spritesheets->insert(spritesheet);
        }

        const auto colliders = cart->getColliders();
        for (const auto &collider: colliders) {
            this->colliders->insert(collider);
        }

        const auto navmeshes = cart->getNavmeshes();
        for (const auto &navmesh: navmeshes) {
            this->navmeshes->insert(navmesh);
        }

        std::cout << "Loaded " << spritesheets.size() << " spritesheets" << std::endl;
        std::cout << "Loaded " << colliders.size() << " colliders" << std::endl;
    }

    Vector2 Runtime::getOffset() const {
        return Vector2{offsetX, offsetY};
    }

    void Runtime::setOffset(float x, float y) {
        offsetX = x;
        offsetY = y;

        canvas->setCamera(x, y);
    }


    std::shared_ptr<graphics::Canvas> Runtime::getCanvas() const {
        return canvas;
    }

    std::shared_ptr<loader::Project> Runtime::getProject() const {
        return project;
    }


    std::shared_ptr<graphics::Font> Runtime::getFont() const {
        return font;
    }

    std::shared_ptr<quickjs::context> Runtime::getContext() const {
        return context;
    }

    std::shared_ptr<std::unordered_map<std::string, graphics::Spritesheet> > Runtime::getSpritesheets() const {
        return spritesheets;
    }

    std::shared_ptr<std::unordered_map<std::string, collision::Collider> > Runtime::getColliders() const {
        return colliders;
    }

    std::shared_ptr<std::unordered_map<std::string, collision::NavMesh> > Runtime::getNavmeshes() const {
        return navmeshes;
    }

    std::shared_ptr<ecs::ECS> Runtime::getECS() const {
        return ecs;
    }

    std::shared_ptr<audio::Audio> Runtime::getAudio() const {
        return audio;
    }

    std::shared_ptr<renderer::Postprocessing> Runtime::getPostprocessing() const {
        return postprocessing;
    }


    void Runtime::setCartridge(std::shared_ptr<Cartridge> cart) {
        cartridge = std::move(cart);
    }

    void Runtime::setProject(std::shared_ptr<loader::Project> project) {
        this->project = std::move(project);
    }

    static const char *c_ident_prefix = "qjsc_";
    static void get_c_name(char *buf, size_t buf_size, const char *file)
    {
        const char *p, *r;
        size_t len, i;
        int c;
        char *q;

        p = strrchr(file, '/');
        if (!p)
            p = file;
        else
            p++;
        r = strrchr(p, '.');
        if (!r)
            len = strlen(p);
        else
            len = r - p;
        pstrcpy(buf, buf_size, c_ident_prefix);
        q = buf + strlen(buf);
        for(i = 0; i < len; i++) {
            c = p[i];
            if (!((c >= '0' && c <= '9') ||
                  (c >= 'A' && c <= 'Z') ||
                  (c >= 'a' && c <= 'z'))) {
                c = '_';
                  }
            if ((q - buf) < buf_size - 1)
                *q++ = c;
        }
        *q = '\0';
    }



    namelist_entry_t *namelist_find(namelist_t *lp, const char *name)
    {
        int i;
        for(i = 0; i < lp->count; i++) {
            namelist_entry_t *e = &lp->array[i];
            if (!strcmp(e->name, name))
                return e;
        }
        return NULL;
    }

    static namelist_t cname_list;

    static void find_unique_cname(char *cname, size_t cname_size)
    {
        char cname1[1024];
        int suffix_num;
        size_t len, max_len;
        assert(cname_size >= 32);
        /* find a C name not matching an existing module C name by
           adding a numeric suffix */
        len = strlen(cname);
        max_len = cname_size - 16;
        if (len > max_len)
            cname[max_len] = '\0';
        suffix_num = 1;
        for(;;) {
            snprintf(cname1, sizeof(cname1), "%s_%d", cname, suffix_num);
            if (!namelist_find(&cname_list, cname1))
                break;
            suffix_num++;
        }
        pstrcpy(cname, cname_size, cname1);
    }

    // TODO: This is crap and should not be here
    JSModuleDef *jsc_module_loader(JSContext *ctx,
                                   const char *module_name, void *opaque) {
        JSModuleDef *m;

        size_t buf_len;
        uint8_t *buf;
        JSValue func_val;
        char cname[1024];

        buf = js_load_file(ctx, &buf_len, module_name);
        if (!buf) {
            JS_ThrowReferenceError(ctx, "could not load module filename '%s'",
                                   module_name);
            return nullptr;
        }

        func_val = JS_Eval(ctx, (char *) buf, buf_len, module_name,
                           JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
        js_free(ctx, buf);
        if (JS_IsException(func_val))
            return nullptr;
        get_c_name(cname, sizeof(cname), module_name);
        if (namelist_find(&cname_list, cname)) {
            find_unique_cname(cname, sizeof(cname));
        }

        /* the module is already referenced, so we must free it */
        m = (JSModuleDef *)JS_VALUE_GET_PTR(func_val);
        JS_FreeValue(ctx, func_val);
        return m;
    }

    void Runtime::init() {
        js_runtime = std::make_unique<quickjs::runtime>();
        context = std::make_shared<quickjs::context>(js_runtime->new_context());
        ecs = std::make_shared<ecs::ECS>(*context);

        quickjs::value global = context->get_global_object();

        js_bindings->registerAll(global, *ecs);

        if (cartridge == nullptr) {
            std::cerr << "No cartridge loaded" << std::endl;
            // Exit the program
            throw std::runtime_error("No cartridge loaded");
        }

        // auto code = cartridge->getCode();

        JS_SetModuleLoaderFunc(&*js_runtime->rt_, nullptr, jsc_module_loader, nullptr);

        auto project = this->project;
        auto entrypoint = project->getEntryPoint();

        std::string code = "import {init, update, draw} from \"" + entrypoint + "\"; globalThis.init = init; globalThis.update = update; globalThis.draw = draw;";

        // let's convert code to escaped string, so it will be possible to save it as a part of JSON:
        // auto code_escaped = nlohmann::json(code).dump();
        // std::cout << code_escaped << std::endl;

        lastTime = std::chrono::steady_clock::now();
        globalTime = 0.0f;

        evalWithStacktrace(code.c_str());
        evalWithStacktrace("init()");
    }

    void Runtime::update() {
        auto currentTime = std::chrono::steady_clock::now();
        const std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update globalTime with the elapsed time
        globalTime += deltaTime.count();


        evalWithStacktrace("update()");
    }

    void Runtime::draw(const RenderTexture2D &renderTexture) const {
        evalWithStacktrace("draw()");
    }

    void Runtime::postProcess(const RenderTexture2D &postProcessTexture, const RenderTexture2D &renderTexture,
                              const Rectangle &srcRect, const Rectangle &destRect,
                              const Vector2 &origin, float rotation, const Color &tint) const {
        // I don't like that this is happening here. Need to rethink the pipeline.
        getCanvas()->applyLighting(postProcessTexture, renderTexture);

        postprocessing->postprocess(postProcessTexture, renderTexture, globalTime, canvasWidth, canvasHeight);
    }

    // TODO: I am not sure this works correctly in all the cases. Perhaps it would be better to handle it with C api directly.
    void Runtime::evalWithStacktrace(const char *code) const {
        try {
            quickjs::value ret = context->eval(
                code,
                "code.js"
            );
        } catch (const quickjs::value_error &e) {
            std::cerr << "JavaScript error: " << e.what() << std::endl;
            if (e.stack()) {
                std::cerr << "Stack trace: " << e.stack() << std::endl;
            }
        } catch (const quickjs::value_exception &e) {
            auto val = e.val();
            if (val.valid()) {
                std::cerr << "JavaScript exception: " << val.as_cstring().c_str() << std::endl;

                // Try to get the stack trace
                auto stack = val.get_property("stack");
                if (stack.valid() && !stack.is_undefined()) {
                    std::cerr << "Stack trace:\n" << stack.as_cstring().c_str() << std::endl;
                } else {
                    std::cerr << "No stack trace available." << std::endl;
                }
            } else {
                std::cerr << "Invalid JavaScript exception value" << std::endl;
            }
        } catch (const quickjs::value_error &e) {
            std::cerr << "JavaScript error: " << e.what() << std::endl;
            if (e.stack()) {
                std::cerr << "Stack trace:\n" << e.stack() << std::endl;
            }
        } catch (const quickjs::exception &e) {
            std::cerr << "QuickJS exception: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "C++ exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception" << std::endl;
        }
    }

    void Runtime::keyDown(Key key) {
        key_flags->setKey(key, true);
    }

    void Runtime::keyUp(Key key) {
        key_flags->setKey(key, false);
    }

    bool Runtime::isKeyPressed(const Key key) const {
        return key_flags->isKeyPressed(key);
    }

    void Runtime::mouseDown(MouseButton button) {
        mouse_state->setButton(button, true);
    }

    void Runtime::mouseUp(MouseButton button) {
        mouse_state->setButton(button, false);
    }

    void Runtime::mouseMove(int x, int y) {
        mouse_state->setPos(x - offsetX, y - offsetY);
    }

    // Vector2 getMousePos(int &x, int &y) const;
    Vector2 Runtime::getMousePos() const {
        return mouse_state->getPos();
    }

    // bool isButtonPressed(MouseButton button) const;
    bool Runtime::isButtonPressed(MouseButton button) const {
        return mouse_state->isButtonPressed(button);
    }
} // runtime
// blipcade
