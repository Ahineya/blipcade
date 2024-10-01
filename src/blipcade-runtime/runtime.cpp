//
// Created by Pavlo Yevsehnieiev
//

#include "runtime.h"

#include <canvas.h>
#include <converters.h>
#include <iostream>
#include <json_cart_data.hpp>
#include <nlohmann/json.hpp>
#include <quickjs.hpp>
#include <utility>

#include "cartridge.h"
#include "JsBindings.h"
#include "keystate.h"

auto const WIDTH = 320;
auto const HEIGHT = 240;

namespace blipcade::runtime {
    Runtime::Runtime(uint32_t width, uint32_t height): canvasWidth(width), canvasHeight(height), cartridge(nullptr),
                                                       canvas(nullptr), key_flags(std::make_shared<Keystate>()),
                                                       mouse_state(std::make_shared<Mousestate>()),
                                                       font(nullptr), js_bindings(std::make_unique<JSBindings>(*this)) {
        canvas = std::make_shared<graphics::Canvas>(canvasWidth, canvasHeight); // TODO: make this configurable
        spritesheets = std::make_shared<std::vector<graphics::Spritesheet> >();

        std::string fontHeader = "40 24 04 06";
        std::string fontData =
                "20 00 00 00 21 00 00 00 22 00 00 00 23 00 00 00 24 00 00 00 25 00 00 00 26 00 00 00 27 00 00 00 28 00 00 00 29 00 00 00 2A 00 00 00 2B 00 00 00 2C 00 00 00 2D 00 00 00 2E 00 00 00 2F 00 00 00 30 00 00 00 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00 39 00 00 00 3A 00 00 00 3B 00 00 00 3C 00 00 00 3D 00 00 00 3E 00 00 00 3F 00 00 00 40 00 00 00 41 00 00 00 42 00 00 00 43 00 00 00 44 00 00 00 45 00 00 00 46 00 00 00 47 00 00 00 48 00 00 00 49 00 00 00 4A 00 00 00 4B 00 00 00 4C 00 00 00 4D 00 00 00 4E 00 00 00 4F 00 00 00 50 00 00 00 51 00 00 00 52 00 00 00 53 00 00 00 54 00 00 00 55 00 00 00 56 00 00 00 57 00 00 00 58 00 00 00 59 00 00 00 5A 00 00 00 5B 00 00 00 5C 00 00 00 5D 00 00 00 5E 00 00 00 5F 00 00 00 60 00 00 00 61 00 00 00 62 00 00 00 63 00 00 00 64 00 00 00 65 00 00 00 66 00 00 00 67 00 00 00 68 00 00 00 69 00 00 00 6A 00 00 00 6B 00 00 00 6C 00 00 00 6D 00 00 00 6E 00 00 00 6F 00 00 00 70 00 00 00 71 00 00 00 72 00 00 00 73 00 00 00 74 00 00 00 75 00 00 00 76 00 00 00 77 00 00 00 78 00 00 00 79 00 00 00 7A 00 00 00 7B 00 00 00 7C 00 00 00 7D 00 00 00 7E 00 00 24 21";
        std::string fontImage =
                "FF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF FF FF FF FF EF FF EF EF FF FF EF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF FF FF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF FF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF EF FF FF FF FF FF FF EF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF FF FF FF FF FF EF FF FF FF EF FF FF EF FF EF FF FF FF FF FF EF FF FF FF FF FF FF FF FF EF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF EF EF EF FF EF FF EF FF FF EF FF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF EF EF FF EF FF EF FF FF EF FF FF EF FF FF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF EF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF EF EF EF FF FF FF EF FF EF EF EF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF EF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF FF FF EF FF FF FF FF EF EF FF FF EF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF EF FF FF FF FF EF FF FF FF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF EF EF FF EF EF FF FF EF EF FF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF EF FF FF FF FF EF FF FF FF FF EF FF FF FF FF FF FF FF FF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF EF FF FF FF FF EF FF FF EF EF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF EF FF FF EF EF EF FF EF EF EF FF FF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF EF FF EF EF FF FF FF EF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF FF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF EF EF FF EF EF FF FF EF FF FF FF EF FF EF FF EF EF FF FF EF EF FF FF EF FF FF FF EF EF EF FF FF EF FF FF FF EF FF FF EF EF FF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF FF FF EF FF FF FF EF FF EF FF EF FF EF FF FF EF FF FF FF EF FF FF EF FF EF FF EF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF FF EF FF EF FF EF EF EF FF FF EF EF FF EF EF EF FF EF EF EF FF EF FF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF FF FF EF FF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF EF EF EF FF FF EF FF FF EF EF EF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF FF FF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF EF FF FF FF EF FF FF EF FF FF FF EF FF FF FF EF FF FF FF FF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF FF FF EF EF EF FF FF EF FF FF EF FF EF FF EF FF EF FF EF FF EF FF FF EF FF FF EF EF EF FF FF EF FF FF EF EF FF FF FF EF FF FF FF EF EF FF EF EF EF FF EF FF EF FF EF FF FF FF EF EF FF FF EF FF EF FF FF FF EF FF FF EF FF FF EF FF EF FF EF EF EF FF EF EF EF FF EF FF EF FF FF FF EF FF EF FF FF FF FF EF FF FF FF EF FF FF FF EF FF FF EF FF FF FF EF FF EF FF EF FF FF FF FF EF EF FF EF FF EF FF EF EF FF FF FF EF FF FF FF EF EF FF FF EF FF FF EF EF EF FF EF FF EF FF EF EF EF FF EF EF EF FF FF EF EF FF FF EF FF FF EF EF FF FF FF FF FF FF EF EF EF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF";

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

        const auto spritesheets = cart->getSpritesheets();
        for (const auto &spritesheet: spritesheets) {
            this->spritesheets->push_back(spritesheet);
        }

        std::cout << "Loaded " << spritesheets.size() << " spritesheets" << std::endl;
    }


    std::shared_ptr<graphics::Canvas> Runtime::getCanvas() const {
        return canvas;
    }

    std::shared_ptr<graphics::Font> Runtime::getFont() const {
        return font;
    }

    std::shared_ptr<quickjs::context> Runtime::getContext() const {
        return context;
    }

    std::shared_ptr<std::vector<graphics::Spritesheet> > Runtime::getSpritesheets() const {
        return spritesheets;
    }

    void Runtime::setCartridge(std::shared_ptr<Cartridge> cart) {
        cartridge = std::move(cart);
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

        auto code = cartridge->getCode();

        // let's convert code to escaped string, so it will be possible to save it as a part of JSON:
        // auto code_escaped = nlohmann::json(code).dump();
        // std::cout << code_escaped << std::endl;

        evalWithStacktrace(code.c_str());
        evalWithStacktrace("init()");
    }

    void Runtime::update() const {
        evalWithStacktrace("update()");
    }

    void Runtime::draw(const RenderTexture2D &renderTexture) const {
        evalWithStacktrace("draw()");
    }

    void Runtime::postProcess(const RenderTexture2D &postProcessTexture, const RenderTexture2D &renderTexture, const Rectangle &srcRect, const Rectangle &destRect,
                              const Vector2 &origin, float rotation, const Color &tint) const {

        // I don't like that this is happening here. Need to rethink the pipeline.
        getCanvas()->applyLighting(postProcessTexture, renderTexture);
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
        mouse_state->setPos(x, y);
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
