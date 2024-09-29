//
// Created by Pavlo Yevsehnieiev
//

#include "JsBindings.h"

#include <canvas.h>
#include <codecvt>
#include <iostream>

#include "runtime.h"

namespace blipcade::runtime {
    JSBindings::JSBindings(Runtime &runtime) : m_runtime(runtime) {
    }

    void JSBindings::registerAll(quickjs::value &global) {
        bindLogFunction(global);
        bindTextFunction(global);

        // Graphics
        bindGraphicsGlobalObject(global);
        bindSetTransparentColor(global);
        bindFillScreen(global);
        bindPutPixel(global);
        bindDrawLine(global);
        bindDrawCircle(global);
        bindDrawFilledCircle(global);
        bindDrawSprite(global);

        // Input
        bindInputGlobalObject(global);
        bindInputIsKeyPressed(global);
        bindInputGetMousePos(global);
        bindInputIsMouseButtonPressed(global);
    }

    /**
     * @namespace global
     * @description These are global functions that can be called from the JavaScript code.
     */

    /**
     * @function log
     * @param {string} [message] - The message to log.
     * @description Logs a message to the console.
     */
    void JSBindings::bindLogFunction(quickjs::value &global) {
        global.set_property("log", [this](const std::string &val) {
            std::cout << "log: " << val << std::endl;
        });
    }

    /**
     * @function text
     * @param {string} text - The text to draw.
     * @param {number} [x=0] - The x-coordinate of the text.
     * @param {number} [y=0] - The y-coordinate of the text.
     * @param {number} [color=0xfe] - The color of the text (default is 0xfe).
     * @description Draws text on the canvas.
     *
     * @returns {void}
     *
     * @example text("Hello, Blipcade!", 10, 20, 0xff);
     */
    void JSBindings::bindTextFunction(quickjs::value &global) {
        global.set_property("text", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                return;
            }

            std::string text = "";
            int x = 0, y = 0, color = 0xfe;

            if (argsCount >= 1) text = a[0].as_string();
            if (argsCount >= 2) x = a[1].as_int32();
            if (argsCount >= 3) y = a[2].as_int32();
            if (argsCount >= 4) color = a[3].as_int32();

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
            std::wstring wide = converter.from_bytes(text);

            m_runtime.getCanvas()->drawText(*m_runtime.getFont(), wide, x, y, color);
        });
    }

    /**
     * @namespace Graphics
     * @description Provides graphics-related functionalities.
     */
    void JSBindings::bindGraphicsGlobalObject(quickjs::value &global) {
        createNamespace(global, "Graphics");
    }

    /**
     *
     * @function setTransparentColor
     * @param {number} [color=0xff] - The color to use as transparent.
     * @description Sets the color to use as transparent when drawing.
     *
     * @example Graphics.setTransparentColor(0x00); // Black color will be transparent
     */
    void JSBindings::bindSetTransparentColor(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("setTransparentColor", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            uint8_t color = 0xff;

            if (argsCount >= 1) color = a[0].as_int32();

            m_runtime.getCanvas()->setTransparentColor(color);
        });
    }

    /**
     * @function fillScreen
     * @param {number} [color=0x00] - The color to fill the screen with.
     * @description Fills the screen with a color.
     * @example Graphics.fillScreen(0xff); // Fills the screen with white color.
     */
    void JSBindings::bindFillScreen(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("fillScreen", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            uint8_t color = 0x00;

            if (argsCount >= 1) color = a[0].as_int32();

            m_runtime.getCanvas()->fillScreen(color);
        });
    }

    /**
     * @function drawSprite
     *
     * @param {number} x - The x-coordinate of the sprite.
     * @param {number} y - The y-coordinate of the sprite.
     * @param {number} spriteIndex - The index of the sprite to draw.
     *
     * @param {number} [spriteSheetIndex=0] - The index of the sprite sheet to use.
     * @param {boolean} [flipX=false] - Whether to flip the sprite horizontally.
     * @param {boolean} [flipY=false] - Whether to flip the sprite vertically.
     *
     * @descriotion Draws a sprite on the canvas.
     *
     * @example Graphics.drawSprite(100, 100, 0); // Draws the first sprite from the first spritesheet at (100, 100).
     *
     */
    void JSBindings::bindDrawSprite(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawSprite", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x = 0, y = 0, spriteIndex = 0;
            uint32_t spriteSheetIndex = 0;
            bool flipX = false, flipY = false;

            if (argsCount >= 1) x = a[0].as_int32();
            if (argsCount >= 2) y = a[1].as_int32();
            if (argsCount >= 3) spriteIndex = a[2].as_int32();
            if (argsCount >= 4) spriteSheetIndex = a[3].as_int32();
            if (argsCount >= 5) flipX = a[4].as_bool();
            if (argsCount >= 6) flipY = a[5].as_bool();

            const auto spritesheets = m_runtime.getSpritesheets();

            if (spriteSheetIndex >= spritesheets->size()) {
                throw std::runtime_error("drawSprite: Invalid sprite sheet index."); // TODO: Throw js error, li
            }

            const auto spritesheet = spritesheets->at(spriteSheetIndex);

            m_runtime.getCanvas()->drawSprite(x, y, flipX, flipY, spritesheet, spriteIndex);
        });
    }

    /**
     * @function putPixel
     *
     * @param {number} x - The x-coordinate of the pixel.
     * @param {number} y - The y-coordinate of the pixel.
     * @param {number} color - The color of the pixel.
     *
     * @description Puts a pixel on the canvas.
     *
     * @example Graphics.putPixel(100, 100, 0xfe); // Puts a pixel at (100, 100) with white color.
     */
    void JSBindings::bindPutPixel(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("putPixel", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x = 0, y = 0;
            uint8_t color = 0x00;

            if (argsCount >= 1) x = a[0].as_int32();
            if (argsCount >= 2) y = a[1].as_int32();
            if (argsCount >= 3) color = a[2].as_int32();

            m_runtime.getCanvas()->drawPixel(x, y, color);
        });
    }

    /**
     * @function drawLine
     *
     * @param {number} x1 - The x-coordinate of the starting point of the line.
     * @param {number} y1 - The y-coordinate of the starting point of the line.
     * @param {number} x2 - The x-coordinate of the ending point of the line.
     * @param {number} y2 - The y-coordinate of the ending point of the line.
     * @param {number} color - The color of the line.
     *
     * @description Draws a line on the canvas.
     *
     * @example Graphics.drawLine(0, 0, 100, 100, 0xfe); // Draws a line from (0, 0) to (100, 100) with white color.
     */
    void JSBindings::bindDrawLine(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawLine", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            uint8_t color = 0x00;

            if (argsCount >= 1) x1 = a[0].as_int32();
            if (argsCount >= 2) y1 = a[1].as_int32();
            if (argsCount >= 3) x2 = a[2].as_int32();
            if (argsCount >= 4) y2 = a[3].as_int32();
            if (argsCount >= 5) color = a[4].as_int32();

            m_runtime.getCanvas()->drawLine(x1, y1, x2, y2, color);
        });
    }

    /**
     * @function drawFilledCircle
     *
     * @param {number} center_x - The x-coordinate of the center of the circle.
     * @param {number} center_y - The y-coordinate of the center of the circle.
     * @param {number} radius - The radius of the circle.
     * @param {number} color - The color of the circle.
     *
     * @description Draws a filled circle on the canvas.
     *
     * @example Graphics.drawFilledCircle(100, 100, 50, 0x50); // Draws a filled circle with a radius of 50 at (100, 100).
     */
    void JSBindings::bindDrawFilledCircle(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawFilledCircle", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t center_x = 0, center_y = 0;
            uint32_t radius = 0;
            uint8_t color = 0x00;

            if (argsCount >= 1) center_x = a[0].as_int32();
            if (argsCount >= 2) center_y = a[1].as_int32();
            if (argsCount >= 3) radius = a[2].as_int32();
            if (argsCount >= 4) color = a[3].as_int32();

            m_runtime.getCanvas()->drawFilledCircle(center_x, center_y, radius, color);
        });
    }

    /**
    * @function drawCircle
    *
    * @param {number} center_x - The x-coordinate of the center of the circle.
    * @param {number} center_y - The y-coordinate of the center of the circle.
    * @param {number} radius - The radius of the circle.
    * @param {number} color - The color of the circle.
    *
    * @description Draws a circle on the canvas.
    *
    * @example Graphics.drawCircle(100, 100, 50, 0x50); // Draws a filled circle with a radius of 50 at (100, 100).
    */
    void JSBindings::bindDrawCircle(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawCircle", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t center_x = 0, center_y = 0;
            uint32_t radius = 0;
            uint8_t color = 0x00;

            if (argsCount >= 1) center_x = a[0].as_int32();
            if (argsCount >= 2) center_y = a[1].as_int32();
            if (argsCount >= 3) radius = a[2].as_int32();
            if (argsCount >= 4) color = a[3].as_int32();

            m_runtime.getCanvas()->drawCircle(center_x, center_y, radius, color);
        });
    }

    /**
     * @namespace Input
     * @description Provides input-related functionalities.
     */
    void JSBindings::bindInputGlobalObject(quickjs::value &global) {
        createNamespace(global, "Input");
    }

    /**
     *
     * @function isKeyPressed
     * @param {number} key - The key code to check.
     *
     * @description Checks if a key is currently pressed. A key codes are defined in the `Key` enum.
     *
     * @return {boolean} - `true` if the key is pressed, `false` otherwise.
     *
     * @example Input.isKeyPressed(Key.ArrowUp); // Returns true if the up arrow key is pressed.
     */
    void JSBindings::bindInputIsKeyPressed(quickjs::value &global) {

        auto input = global.get_property("Input");

        input.set_property("isKeyPressed", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("isKeyPressed: Missing argument.");
            }

            int key = a[0].as_int32();
            auto isKeyPressed = m_runtime.isKeyPressed(static_cast<Key>(key));
            return {*ctx, isKeyPressed};

        });
    }

    /**
     * @function getMousePos
     *
     * @description Gets the current mouse position.
     *
     * @returns {object} - An object with `x` and `y` properties representing the mouse position.
     *
     * @example Input.getMousePos(); // Returns an object with `x` and `y` properties representing the mouse position.
     */
    void JSBindings::bindInputGetMousePos(quickjs::value &global) {
        auto input = global.get_property("Input");

        input.set_property("getMousePos", [this, global](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto mousePos = m_runtime.getMousePos();

            quickjs::value Object = global.get_property("Object");
            quickjs::value obj = Object.call_member("create", quickjs::value::null(*ctx));

            const quickjs::value mouseX(*ctx, static_cast<double>(mousePos.x));
            const quickjs::value mouseY(*ctx, static_cast<double>(mousePos.y));

            obj.set_property("x", mouseX);
            obj.set_property("y", mouseY);

            return obj;
        });
    }

    /**
     * @function isMouseButtonPressed
     *
     * @param {number} button - The button code to check. 1 for left, 2 for right, 3 for middle.
     *
     * @returns {boolean} - `true` if the button is pressed, `false` otherwise.
     *
     * @example Input.isMouseButtonPressed(1); // Returns true if the left mouse button is pressed.
     */
    void JSBindings::bindInputIsMouseButtonPressed(quickjs::value &global) {
        auto input = global.get_property("Input");

        input.set_property("isMouseButtonPressed", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("isMouseButtonPressed: Missing argument.");
            }

            int button = a[0].as_int32();
            auto isMouseButtonPressed = m_runtime.isButtonPressed(static_cast<MouseButton>(button));
            return {*ctx, isMouseButtonPressed};
        });
    }

    // helper crap
    void JSBindings::createNamespace(quickjs::value &global, const std::string &name) {
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        quickjs::value Object = global.get_property("Object");
        const quickjs::value obj = Object.call_member("create", quickjs::value::null(*ctx));

        global.set_property(name, obj);
    }

} // runtime
// blipcade
