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

    void JSBindings::registerAll(quickjs::value &global, ecs::ECS& ecs) {
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

        // ECS
        bindECSGlobalObject(global, ecs);
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

            if (argsCount >= 1) {
                auto ab = a[0].as_cstring();
                text = ab.c_str();
            }

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
            if (argsCount >= 5) flipX = a[4].as_int32();
            if (argsCount >= 6) flipY = a[5].as_int32();

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

    /**
     * @namespace ECS
     * @description Provides Entity-Component-System functionalities.
     */
    void JSBindings::bindECSGlobalObject(quickjs::value &global, ecs::ECS &ecs) {
        createNamespace(global, "ECS");

        bindCreateEntity(global, ecs);
        bindDestroyEntity(global, ecs);
        bindAddComponent(global, ecs);
        bindRemoveComponent(global, ecs);
        bindGetComponent(global, ecs);
        bindForEachEntity(global, ecs);
    }

    /**
     * @function createEntity
     * @description Creates a new entity.
     * @returns {number} - The ID of the created entity.
     *
     * @example const entity = ECS.createEntity(); // Creates a new entity.
     */
    void JSBindings::bindCreateEntity(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("createEntity", [&ecs, &ctx](const quickjs::args &a) {
            auto const entity = ecs.createEntity();
            return entity;
        });
    }

    /**
     * @function destroyEntity
     * @param {number} entity - The ID of the entity to destroy.
     * @description Destroys an entity.
     *
     * @example ECS.destroyEntity(entity); // Destroys the entity with the given ID.
     */
    void JSBindings::bindDestroyEntity(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("destroyEntity", [&ecs, &ctx](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("destroyEntity: Missing argument.");
            }

            auto entity = a[0].as_uint32();
            ecs.destroyEntity(entity);
        });
    }

    /**
     * @function addComponent
     * @param {number} entity - The ID of the entity to add the component to.
     * @param {string} typeName - The name of the component type.
     * @param {object} component - The component to add.
     * @description Adds a component to an entity.
     *
     * @example ECS.addComponent(entity, "Position", { x: 10, y: 20 }); // Adds a Position component to the entity.
     */
    void JSBindings::bindAddComponent(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("addComponent", [&ecs, &ctx](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 3) {
                throw std::runtime_error("addComponent: Missing arguments.");
            }

            auto entity = a[0].as_uint32();
            auto typeName = a[1].as_cstring().c_str();
            auto component = a[2];

            // std::cout << "Adding component: " << typeName << std::endl;

            ecs.addComponent(entity, typeName, component);

            // std::cout << "Component added: " << typeName << std::endl;
        });
    }

    /**
     * @function removeComponent
     * @param {number} entity - The ID of the entity to remove the component from.
     * @param {string} typeName - The name of the component type.
     * @description Removes a component from an entity.
     *
     * @example ECS.removeComponent(entity, "Position"); // Removes the Position component from the entity.
     */
    void JSBindings::bindRemoveComponent(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("removeComponent", [&ecs, &ctx](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("removeComponent: Missing arguments.");
            }

            auto entity = a[0].as_uint32();
            auto typeName = a[1].as_cstring().c_str();

            ecs.removeComponent(entity, typeName);
        });
    }

    /**
     * @function getComponent
     * @param {number} entity - The ID of the entity to get the component from.
     * @param {string} typeName - The name of the component type.
     * @description Gets a component from an entity.
     * @returns {object} - The component.
     *
     * @example const position = ECS.getComponent(entity, "Position"); // Gets the Position component from the entity.
     */
    void JSBindings::bindGetComponent(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("getComponent", [&ecs, &ctx](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("getComponent: Missing arguments.");
            }

            auto entity = a[0].as_uint32();
            auto typeName = a[1].as_cstring().c_str();

            auto component = ecs.getComponent(entity, typeName);
            return component;
        });
    }

    /**
     * @function forEachEntity
     * @param {Array} componentTypes - An array of string component types to filter entities by.
     * @param {function} callback - The callback function to call for each entity.
     * @param {boolean} [reverse=false] - Whether to iterate over entities in reverse order.
     * @description Iterates over entities that have all the specified components.
     *
     * @example
     * ECS.forEachEntity(["Position", "Velocity"], (entity) => {
     *     const position = ECS.getComponent(entity, "Position");
     *     const velocity = ECS.getComponent(entity, "Velocity");
     *     // Do something with the position and velocity components
     * });
     */
    void JSBindings::bindForEachEntity(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("forEachEntity", [&ecs, ctx](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw quickjs::throw_exception(
                    quickjs::value::type_error(a.get_context(), "forEachEntity: Missing arguments.")
                );
            }

            quickjs::value componentTypesValue = a[0];

            // Check if the first argument is an array
            if (!componentTypesValue.is_array()) {
                throw quickjs::throw_exception(
                    quickjs::value::type_error(a.get_context(), "First argument must be an array.")
                );
            }

            // Get the length of the array
            quickjs::value lengthValue = componentTypesValue.get_property("length");
            uint32_t length = lengthValue.as_uint32();

            // Iterate over the array elements
            std::vector<std::string> types;
            for (uint32_t i = 0; i < length; ++i) {
                quickjs::value element = componentTypesValue.get_property(i);

                if (!element.is_string()) {
                    throw quickjs::throw_exception(
                        quickjs::value::type_error(a.get_context(), "Array elements must be strings.")
                    );
                }

                types.push_back(element.as_cstring().c_str());
            }

            quickjs::value callback = a[1];

            // If there is a third argument, check if it's a boolean
            bool reverse = false;
            if (argsCount >= 3) {
                quickjs::value reverseValue = a[2];
                if (!reverseValue.is_bool()) {
                    throw quickjs::throw_exception(
                        quickjs::value::type_error(a.get_context(), "Third argument must be a boolean.")
                    );
                }

                reverse = reverseValue.as_int32();
            }

            ecs.forEachEntity(types, callback, reverse);
        });
    }

// #include "quickjspp.hpp"
// #include "ECS.hpp"
//
//     void registerECS(qjs::Context& context, ECS& ecs) {
//         context.global().add("createEntity", [&ecs]() {
//             return ecs.createEntity();
//         });
//
//         context.global().add("destroyEntity", [&ecs](Entity entity) {
//             ecs.destroyEntity(entity);
//         });
//
//         context.global().add("addComponent", [&ecs](Entity entity, const std::string& typeName, qjs::Value component) {
//             ecs.addComponent(entity, typeName, component.v);
//         });
//
//         context.global().add("removeComponent", [&ecs](Entity entity, const std::string& typeName) {
//             ecs.removeComponent(entity, typeName);
//         });
//
//         context.global().add("getComponent", [&ecs](Entity entity, const std::string& typeName) {
//             JSValue comp = ecs.getComponent(entity, typeName);
//             return qjs::Value(context, comp);
//         });
//
//         context.global().add("forEachEntity", [&ecs](std::vector<std::string> componentTypes, qjs::Value callback) {
//             ecs.forEachEntity(componentTypes, callback.v);
//         });
//     }



} // runtime
// blipcade
