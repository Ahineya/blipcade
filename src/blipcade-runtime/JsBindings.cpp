//
// Created by Pavlo Yevsehnieiev
//

#include "JsBindings.h"

#include <audio.h>
#include <canvas.h>
#include <codecvt>
#include <collider.h>
#include <iostream>
#include <navmesh.h>
#include <pathfinding.h>
#include <project.h>

#include "runtime.h"

namespace blipcade::runtime {
    JSBindings::JSBindings(Runtime &runtime) : m_runtime(runtime) {
    }

    void JSBindings::registerAll(quickjs::value &global, ecs::ECS &ecs) {
        bindLogFunction(global);
        bindTextFunction(global);

        // Blip functions
        bindBlipFunctions(global);

        // Graphics
        bindGraphicsGlobalObject(global);
        bindSetTransparentColor(global);
        bindFillScreen(global);
        bindPutPixel(global);
        bindDrawLine(global);
        bindDrawCircle(global);
        bindDrawFilledCircle(global);
        bindDrawFilledRectangle(global);
        bindDrawSprite(global);
        bindDrawSpriteEx(global);

        // Lighting
        bindLightingGlobalObject(global);
        bindAddLightEffect(global);
        bindRemoveLightEffect(global);
        bindUpdateLightEffect(global);
        bindChangeLightOpacity(global);
        bindChangeLightTintColor(global);

        // Input
        bindInputGlobalObject(global);
        bindInputIsKeyPressed(global);
        bindInputGetMousePos(global);
        bindInputIsMouseButtonPressed(global);

        // ECS
        bindECSMethods(global, ecs);

        // Collision Detection and pathfinding
        bindCollisionDetectionMethods(global);
        bindPathfindingMethods(global);

        // Sound
        bindSoundMethods(global);
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
     * @namespace Blip
     * @description Provides engine functionalities.
     */
    void JSBindings::bindBlipFunctions(quickjs::value &global) {
        createNamespace(global, "Blip");

        bindLoadSpritesheet(global);
        bindLoadNavmesh(global);
    }

    /**
     * @function loadSpritesheet
     * @param {string} path - The path to the spritesheet.
     * @description Loads a spritesheet from a file.
     *
     * @example Blip.loadSpritesheet("res://assets/spritesheet.png");
     */
    void JSBindings::bindLoadSpritesheet(quickjs::value &global) {
        auto blip = global.get_property("Blip");

        blip.set_property("loadSpritesheet", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("loadSpritesheet: Missing argument.");
            }

            std::string path = a[0].as_cstring().c_str();

            auto spritesheets = m_runtime.getSpritesheets();

            if (spritesheets->find(path) != spritesheets->end()) {
                return;
            }

            auto spritesheet = graphics::Spritesheet::fromResource(path, m_runtime.getProject()->getDirectory());

            spritesheets->insert({path, spritesheet});
        });
    }

    /**
     * @function loadNavmesh
     * @param {string} path - The path to the navmesh.
     * @description Loads a navmesh from a file.
     *
     * @example Blip.loadNavmesh("res://assets/navmesh.json");
     */
    void JSBindings::bindLoadNavmesh(quickjs::value &global) {
        auto blip = global.get_property("Blip");

        blip.set_property("loadNavmesh", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("loadNavmesh: Missing argument.");
            }

            std::string path = a[0].as_cstring().c_str();

            auto navmeshes = m_runtime.getNavmeshes();

            if (navmeshes->find(path) != navmeshes->end()) {
                return;
            }

            auto navmesh = collision::NavMesh::fromResource(path, m_runtime.getProject()->getDirectory());

            navmeshes->insert({path, navmesh});
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
     * @param {string} [spriteSheet=""] - The path of the sprite sheet to use.
     * @param {boolean} [flipX=false] - Whether to flip the sprite horizontally.
     * @param {boolean} [flipY=false] - Whether to flip the sprite vertically.
     *
     * @description Draws a sprite on the canvas.
     *
     * @example Graphics.drawSprite(100, 100, 0); // Draws the first sprite from the first spritesheet at (100, 100).
     *
     */
    void JSBindings::bindDrawSprite(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawSprite", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x = 0, y = 0, spriteIndex = 0;
            std::string spriteSheetIndex = "0";
            bool flipX = false, flipY = false;

            if (argsCount >= 1) x = a[0].as_int32();
            if (argsCount >= 2) y = a[1].as_int32();
            if (argsCount >= 3) spriteIndex = a[2].as_int32();
            if (argsCount >= 4) spriteSheetIndex = a[3].as_cstring().c_str();
            if (argsCount >= 5) flipX = a[4].as_int32();
            if (argsCount >= 6) flipY = a[5].as_int32();

            const auto spritesheets = m_runtime.getSpritesheets();

            if (spritesheets->find(spriteSheetIndex) == spritesheets->end()) {
                std::cout << "drawSprite: Loading spritesheet from resource: " << spriteSheetIndex << std::endl;
                auto spritesht = graphics::Spritesheet::fromResource(spriteSheetIndex, m_runtime.getProject()->getDirectory());
                m_runtime.getSpritesheets()->insert({spriteSheetIndex, spritesht});
            }

            const auto spritesheet = spritesheets->at(spriteSheetIndex);

            m_runtime.getCanvas()->drawSprite(x, y, flipX, flipY, spritesheet, spriteIndex);
        });
    }

    //
    //void Canvas::drawSpriteEx(int32_t x, int32_t y, bool flipX, bool flipY, float wChange, float hChange,
    //const Spritesheet &spritesheet, uint32_t index
    //)

    /**
     * @function drawSpriteEx
     *
     * @param {number} x - The x-coordinate of the sprite.
     * @param {number} y - The y-coordinate of the sprite.
     * @param {number} spriteIndex - The index of the sprite to draw.
     *
     * @param {number} [spriteSheetIndex=0] - The index of the sprite sheet to use.
     * @param {boolean} [flipX=false] - Whether to flip the sprite horizontally.
     * @param {boolean} [flipY=false] - Whether to flip the sprite vertically.
     *
     * @param {number} [scale=1.0] - The scale of the sprite.
     *
     * @param {number} [originX=0.5] - The x origin of the sprite, from 0 to 1, where 0 is the left and 1 is the right.
     * @param {number} [originY=0.5] - The y origin of the sprite, from 0 to 1, where 0 is the top and 1 is the bottom.
     *
     * @description Draws a sprite on the canvas.
     *
     * @example Graphics.drawSpriteEx(100, 100, 0, 0, false, false, 1.0, 1.0); // Draws the first sprite from the first spritesheet at (100, 100).
     *
     */
    void JSBindings::bindDrawSpriteEx(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawSpriteEx", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x = 0, y = 0, spriteIndex = 0;
            std::string spriteSheetIndex = "";
            bool flipX = false, flipY = false;
            float scale = 1.0, originX = 0.5, originY = 0.5;

            if (argsCount >= 1) x = a[0].as_int32();
            if (argsCount >= 2) y = a[1].as_int32();
            if (argsCount >= 3) spriteIndex = a[2].as_int32();
            if (argsCount >= 4) spriteSheetIndex = a[3].as_cstring().c_str();
            if (argsCount >= 5) flipX = a[4].as_int32();
            if (argsCount >= 6) flipY = a[5].as_int32();
            if (argsCount >= 7) scale = a[6].as_double();
            if (argsCount >= 8) originX = a[7].as_double();
            if (argsCount >= 9) originY = a[8].as_double();

            const auto spritesheets = m_runtime.getSpritesheets();

            if (spritesheets->find(spriteSheetIndex) == spritesheets->end()) {
                std::cout << "drawSprite: Loading spritesheet from resource: " << spriteSheetIndex << std::endl;
                auto spritesht = graphics::Spritesheet::fromResource(spriteSheetIndex, m_runtime.getProject()->getDirectory());
                m_runtime.getSpritesheets()->insert({spriteSheetIndex, spritesht});
            }

            const auto spritesheet = spritesheets->at(spriteSheetIndex);

            m_runtime.getCanvas()->drawSpriteEx(x, y, flipX, flipY, scale, originX, originY, spritesheet, spriteIndex);
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
     *
     * @function drawFilledRect
     *
     * @param {number} x - The x-coordinate of the top-left corner of the rectangle.
     * @param {number} y - The y-coordinate of the top-left corner of the rectangle.
     * @param {number} width - The width of the rectangle.
     * @param {number} height - The height of the rectangle.
     *
     * @param {number} color - The color of the rectangle.
     *
     * @description Draws a filled rectangle on the canvas.
     *
     * @example Graphics.drawFilledRect(100, 100, 50, 50, 0x50); // Draws a filled rectangle with a width and height of 50 at (100, 100) with color 0x50.
     */
    void JSBindings::bindDrawFilledRectangle(quickjs::value &global) {
        auto graphics = global.get_property("Graphics");

        graphics.set_property("drawFilledRect", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            int32_t x = 0, y = 0, width = 0, height = 0;
            uint8_t color = 0xfe;

            if (argsCount >= 1) x = a[0].as_int32();
            if (argsCount >= 2) y = a[1].as_int32();
            if (argsCount >= 3) width = a[2].as_int32();
            if (argsCount >= 4) height = a[3].as_int32();
            if (argsCount >= 5) color = a[4].as_int32();

            m_runtime.getCanvas()->drawFilledRectangleW(x, y, width, height, color);
        });
    }

    /**
    * @function drawCircle
    *
    * @param {number} center_x - The x-coordinate of the center of the circle.
    * @param {number} center_y - The y-coordinate of the center of the circle.
    * @param {number} radius - The radius of the circle.
    * @param {number} color - The color of the circle.
    * @param {object} params - Additional parameters for the circle.
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
     * @namespace Lighting
     * @description Provides lighting-related functionalities.
     */
    void JSBindings::bindLightingGlobalObject(quickjs::value &global) {
        createNamespace(global, "Lighting");
    }

    /**
     * @function addLightEffect
     * @param {string} name - Unique name for the light effect.
     * @param {object} params - Parameters for the light effect.
     * @param {number} params.tintColor - The tint color in hexadecimal (e.g., 0xff0000 for red).
     * @param {number} params.opacity - Opacity of the tint (0.0 to 1.0).
     * @param {string} params.maskImagePath - Path to the mask image.
     * @description Adds a new light effect.
     *
     * @example
     * Lighting.addLightEffect("sunlight", {
     *     tintColor: 0xffe066,
     *     opacity: 0.5,
     *     maskImagePath: "assets/masks/trapezoid.png"
     * });
     */
    void JSBindings::bindAddLightEffect(quickjs::value &global) {
        auto lighting = global.get_property("Lighting");

        lighting.set_property("addLightEffect", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("addLightEffect: Missing arguments.");
            }

            std::string name = a[0].as_cstring().c_str();
            quickjs::value params = a[1];

            // Extract parameters
            uint32_t tintColorHex = 0xffffff;
            float opacity = 1.0f;
            std::string maskImagePath = "";

            // Check and extract 'tintColor'
            quickjs::value tintColorVal = params.get_property("tintColor");
            if (!tintColorVal.is_undefined()) {
                tintColorHex = tintColorVal.as_uint32();
            }

            // Check and extract 'opacity'
            quickjs::value opacityVal = params.get_property("opacity");
            if (!opacityVal.is_undefined()) {
                opacity = opacityVal.as_double();
            }

            // Check and extract 'maskImagePath'
            quickjs::value maskImagePathVal = params.get_property("maskImagePath");
            if (!maskImagePathVal.is_undefined()) {
                maskImagePath = maskImagePathVal.as_cstring().c_str();
            }

            // Convert hex color to Color
            Color tintColor = {
                static_cast<uint8_t>((tintColorHex >> 16) & 0xFF),
                static_cast<uint8_t>((tintColorHex >> 8) & 0xFF),
                static_cast<uint8_t>(tintColorHex & 0xFF),
                255
            };

            // Load mask texture
            Texture2D maskTexture = LoadTexture(maskImagePath.c_str());
            if (maskTexture.id == 0) {
                std::cerr << "Error: Failed to load mask texture from path: " << maskImagePath << std::endl;
                throw std::runtime_error("addLightEffect: Failed to load mask texture.");
            }
            // Create LightEffect
            graphics::LightEffect effect = {
                tintColor,
                opacity,
                maskTexture
            };

            // Add to Canvas
            m_runtime.getCanvas()->addLightEffect(name, effect);
        });
    }

    /**
     * @function removeLightEffect
     * @param {string} name - The name of the light effect to remove.
     * @description Removes an existing light effect.
     *
     * @example
     * Lighting.removeLightEffect("sunlight");
     */
    void JSBindings::bindRemoveLightEffect(quickjs::value &global) {
        auto lighting = global.get_property("Lighting");

        lighting.set_property("removeLightEffect", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("removeLightEffect: Missing argument.");
            }

            std::string name = a[0].as_cstring().c_str();
            m_runtime.getCanvas()->removeLightEffect(name);
        });
    }

    /**
     * @function updateLightEffect
     * @param {string} name - The name of the light effect to update.
     * @param {object} params - Updated parameters for the light effect.
     * @param {number} [params.tintColor] - The new tint color in hexadecimal.
     * @param {number} [params.opacity] - New opacity of the tint.
     * @param {string} [params.maskImagePath] - New path to the mask image.
     * @description Updates an existing light effect.
     *
     * @example
     * Lighting.updateLightEffect("sunlight", {
     *     tintColor: 0xffffff,
     *     opacity: 0.7
     * });
     */
    void JSBindings::bindUpdateLightEffect(quickjs::value &global) {
        auto lighting = global.get_property("Lighting");

        lighting.set_property("updateLightEffect", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("updateLightEffect: Missing arguments.");
            }

            std::string name = a[0].as_cstring().c_str();
            quickjs::value params = a[1];

            // Extract existing light effect
            // (Assuming you have a method to retrieve existing light effects)
            // For simplicity, let's assume we recreate the effect
            // In practice, you'd retrieve and update the existing one

            uint32_t tintColorHex = 0xffffff;
            float opacity = 1.0f;
            std::string maskImagePath = "";

            quickjs::value tintColorVal = params.get_property("tintColor");
            if (!tintColorVal.is_undefined()) {
                tintColorHex = tintColorVal.as_uint32();
            }

            // Check and extract 'opacity'
            quickjs::value opacityVal = params.get_property("opacity");
            if (!opacityVal.is_undefined()) {
                opacity = opacityVal.as_double();
            }

            // Check and extract 'maskImagePath'
            quickjs::value maskImagePathVal = params.get_property("maskImagePath");
            if (!maskImagePathVal.is_undefined()) {
                maskImagePath = maskImagePathVal.as_cstring().c_str();
            }

            // Convert hex color to Color
            // Convert hex color to Color
            Color tintColor = {
                static_cast<uint8_t>((tintColorHex >> 16) & 0xFF),
                static_cast<uint8_t>((tintColorHex >> 8) & 0xFF),
                static_cast<uint8_t>(tintColorHex & 0xFF),
                255
            };

            // Load mask texture if provided
            Texture2D maskTexture;
            if (!maskImagePath.empty()) {
                maskTexture = LoadTexture(maskImagePath.c_str());
            } else {
                // Retrieve existing mask texture or set a default
                // For simplicity, let's assume it's already loaded
                // Otherwise, you might need to handle this case
            }

            // Create LightEffect
            graphics::LightEffect effect = {
                tintColor,
                opacity,
                maskTexture
            };

            // Update in Canvas
            m_runtime.getCanvas()->updateLightEffect(name, effect);
        });
    }

    /**
     * @function changeLightOpacity
     *
     * @param {string} name - The name of the light effect to update.
     * @param {number} opacity - New opacity of the tint.
     *
     * @description Updates the opacity of an existing light effect.
     *
     * @example Lighting.changeLightOpacity("sunlight", 0.7);
     */
    void JSBindings::bindChangeLightOpacity(quickjs::value &global) {
        auto lighting = global.get_property("Lighting");

        lighting.set_property("changeLightOpacity", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("changeLightOpacity: Missing arguments.");
            }

            std::string name = a[0].as_cstring().c_str();
            float opacity = a[1].as_double();

            // Update in Canvas
            m_runtime.getCanvas()->setLightOpacity(name, opacity);
        });
    }

    /**
     *
     * @function changeLightTintColor
     *
     * @param {string} name - The name of the light effect to update.
     * @param {number} tintColor - The new tint color in hexadecimal.
     *
     * @description Updates the tint color of an existing light effect.
     *
     * @example Lighting.changeLightTintColor("sunlight", 0xff0000); // Changes the tint color to red.
     */
    void JSBindings::bindChangeLightTintColor(quickjs::value &global) {
        auto lighting = global.get_property("Lighting");

        lighting.set_property("changeLightTintColor", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("changeLightTintColor: Missing arguments.");
            }

            std::string name = a[0].as_cstring().c_str();
            uint32_t tintColorHex = a[1].as_uint32();

            // Convert hex color to Color
            Color tintColor = {
                static_cast<uint8_t>((tintColorHex >> 16) & 0xFF),
                static_cast<uint8_t>((tintColorHex >> 8) & 0xFF),
                static_cast<uint8_t>(tintColorHex & 0xFF),
                255
            };

            // Update in Canvas
            m_runtime.getCanvas()->setLightTintColor(name, tintColor);
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
    void JSBindings::bindECSMethods(quickjs::value &global, ecs::ECS &ecs) {
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

        ECS.set_property("createEntity", [&ecs](const quickjs::args &a) {
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

        ECS.set_property("destroyEntity", [&ecs](const quickjs::args &a) {
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

        ECS.set_property("removeComponent", [&ecs](const quickjs::args &a) {
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

        ECS.set_property("getComponent", [&ecs](const quickjs::args &a) {
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
     * @param {function} callback - The callback function to call for each entity. The first argument is the entity ID. Subsequent arguments are the components in the order specified in the `componentTypes` array.
     * @param {boolean} [reverse=false] - Whether to iterate over entities in reverse order.
     * @description Iterates over entities that have all the specified components. Iteration order is guaranteed to be the same as the order in which entities were created.
     *
     * @example ECS.forEachEntity(["Position", "Velocity"], (entity, position, velocity) => { ... }
     */
    void JSBindings::bindForEachEntity(quickjs::value &global, ecs::ECS &ecs) {
        auto ECS = global.get_property("ECS");
        const std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        ECS.set_property("forEachEntity", [&ecs](const quickjs::args &a) {
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

    /**
     * @namespace Collision
     * @description Provides collision-related functionalities.
     */
    void JSBindings::bindCollisionDetectionMethods(quickjs::value &global) {
        createNamespace(global, "Collision");

        bindGetCollider(global);
        // bindCheckCollision(global);
        bindCheckCollisionPoint(global);
        // bindCheckCollisionCircle(global);
        // bindCheckCollisionCircleRec(global);
        // bindCheckCollisionRecs(global);
    }

    /**
     * @function getCollider
     *
     * @param {string} resourcePath - The path of the resource of the collider to get.
     *
     * @description Gets the collider object at the specified index. It has ['type', 'vertices', 'triangles'] properties.
     *
     * @returns {object} - The collider object. It has ['type', 'vertices', 'triangles'] properties.
     *
     * @example const collider = Collision.getCollider("res://colliders/collider.json"); // Gets the collider object at index 0.
     */
    void JSBindings::bindGetCollider(quickjs::value &global) {
        auto collision = global.get_property("Collision");

        collision.set_property("getCollider", [this, global](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto mousePos = m_runtime.getMousePos();

            quickjs::value Object = global.get_property("Object");
            quickjs::value obj = Object.call_member("create", quickjs::value::null(*ctx));

            const auto colliders = m_runtime.getColliders();

            if (a.size() < 1) {
                throw std::runtime_error("getCollider: Missing argument.");
            }

            auto colliderIndex = "0";

            if (a.size() >= 1) {
                colliderIndex = a[0].as_cstring().c_str();
            }

            if (colliders->find(colliderIndex) == colliders->end()) {
                // throw std::runtime_error("checkCollisionPoint: Collider not found.");
                std::cout << "checkCollisionPoint: Loading collider from resource: " << colliderIndex << std::endl;
                auto collider = collision::Collider::fromResource(colliderIndex, m_runtime.getProject()->getDirectory());
                std::cout << "Loaded collider from resource: " << colliderIndex << std::endl;
                m_runtime.getColliders()->insert({colliderIndex, collider});
                std::cout << "Inserted collider: " << colliderIndex << std::endl;
            }

            auto collider = colliders->at(colliderIndex);

            const quickjs::value type(*ctx, static_cast<double>(collider.type));
            // const quickjs::value vertices(*ctx, collider.vertices);

            quickjs::value Array = ctx->get_global_object().get_property("Array");
            quickjs::value arrayLength = Object.call_member("create", quickjs::value::null(*ctx));
            arrayLength.set_property("length", static_cast<double>(collider.vertices.size()));
            // quickjs::value vertices = Array.call_member("from", arrayLength, );

            quickjs::value vertices = Array.call_member("from", 0);

            for (size_t i = 0; i < collider.vertices.size(); i++) {
            quickjs::value vertice = Object.call_member("create", quickjs::value::null(*ctx));

            vertice.set_property("x", static_cast<double>(collider.vertices[i].x));
            vertice.set_property("y", static_cast<double>(collider.vertices[i].y));

            vertices.call_member("push", vertice);
            }

            // const quickjs::value triangles(*ctx, collider.triangles);

            obj.set_property("type", type);
            obj.set_property("vertices", vertices);
            // obj.set_property("triangles", triangles);

            return obj;
        });
    }

    /**
     * @function checkCollisionPoint
     *
     * @param {number} x - The x-coordinate of the point to check.
     * @param {number} y - The y-coordinate of the point to check.
     * @param {string} resourcePath - The path of the resource of the collider to check.
     *
     * @description Checks if a point collides with a collider.
     *
     * @returns {boolean} - `true` if the point collides with the collider, `false` otherwise.
     *
     * @example Collision.checkCollisionPoint(100, 100, 0); // Checks if the point (100, 100) collides with the collider at index 0.
     */
    void JSBindings::bindCheckCollisionPoint(quickjs::value &global) {
        auto collision = global.get_property("Collision");

        collision.set_property("checkCollisionPoint", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto argsCount = a.size();

            if (argsCount < 3) {
                throw std::runtime_error("checkCollisionPoint: Missing arguments.");
            }

            auto x = a[0].as_double();
            auto y = a[1].as_double();
            auto colliderIndex = a[2].as_cstring().c_str();

            auto colliders = m_runtime.getColliders();

            if (colliders->find(colliderIndex) == colliders->end()) {
                // throw std::runtime_error("checkCollisionPoint: Collider not found.");
                std::cout << "checkCollisionPoint: Loading collider from resource: " << colliderIndex << std::endl;
                auto collider = collision::Collider::fromResource(colliderIndex, m_runtime.getProject()->getDirectory());
                m_runtime.getColliders()->insert({colliderIndex, collider});
            }

            auto collider = colliders->at(colliderIndex);

            auto point = Vector2(x, y);
            auto result = collider.checkCollisionPoint(point);

            return quickjs::value(*ctx, result);
        });
    }

    /**
     * @namespace Pathfinding
     *
     * @description Provides pathfinding-related functionalities.
     */
    void JSBindings::bindPathfindingMethods(quickjs::value &global) {
        createNamespace(global, "Pathfinding");

        bindFindPath(global);
        bindGetNavMesh(global);
    }

    /**
     * @function findPath
     *
     * @param {number} startX - The x-coordinate of the starting point.
     * @param {number} startY - The y-coordinate of the starting point.
     * @param {number} endX - The x-coordinate of the ending point.
     * @param {number} endY - The y-coordinate of the ending point.
     * @param {string} navigationMeshPath - The path the navigation mesh resource to use.
     *
     * @description Finds a path from the starting point to the ending point using the specified navigation mesh.
     *
     * @returns {Array} - An array of points representing the path. Each point is an object with `x` and `y` properties.
     *
     * @example Pathfinding.findPath(0, 0, 100, 100, "res://navmesh.json"); // Finds a path from (0, 0) to (100, 100) using the navigation mesh with the specified path.
     */
    void JSBindings::bindFindPath(quickjs::value &global) {
        auto pathfinding = global.get_property("Pathfinding");

        pathfinding.set_property("findPath", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto argsCount = a.size();

            if (argsCount < 5) {
                std::cout << "argsCount: " << argsCount << std::endl;
                throw std::runtime_error("findPath: Missing arguments.");
            }

            auto startX = a[0].as_int32();
            auto startY = a[1].as_int32();
            auto endX = a[2].as_int32();
            auto endY = a[3].as_int32();
            auto navigationMeshId = a[4].as_cstring().c_str();

            auto navmeshes = m_runtime.getNavmeshes();

            if (navmeshes->find(navigationMeshId) == navmeshes->end()) {
                // throw std::runtime_error("findPath: Navigation mesh not found.");
                std::cout << "findPath: Loading navmesh from resource: " << navigationMeshId << std::endl;
                auto nm = collision::NavMesh::fromResource(navigationMeshId, m_runtime.getProject()->getDirectory());
                m_runtime.getNavmeshes()->insert({navigationMeshId, nm});
            }

            auto navMesh = navmeshes->at(navigationMeshId);

            auto path = collision::Pathfinding::pathfind(startX, startY, endX, endY, navMesh, true);

            quickjs::value Object = ctx->get_global_object().get_property("Object");
            quickjs::value Array = ctx->get_global_object().get_property("Array");
            quickjs::value pathArray = Array.call_member("from", 0);
            for (const auto &point : path) {
                quickjs::value pointObj = Object.call_member("create", quickjs::value::null(*ctx));

                pointObj.set_property("x", static_cast<double>(point.x));
                pointObj.set_property("y", static_cast<double>(point.y));

                pathArray.call_member("push", pointObj);
            }

            return pathArray;
        });
    }

    /**
     * @function getNavMesh
     *
     * @param {string} resourcePath - The ID of the navigation mesh to get.
     *
     * @description Gets the navigation mesh with the specified path.
     *
     * @returns {Array} - An array of regions in the navigation mesh. Each region is an object with a `vertices` property containing an array of points.
     *
     * @example Pathfinding.getNavMesh("res://navmesh/navmesh.json"); // Gets the navigation mesh with the specified path.
     */
    void JSBindings::bindGetNavMesh(quickjs::value &global) {
        auto pathfinding = global.get_property("Pathfinding");

        pathfinding.set_property("getNavMesh", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            if (a.size() < 1) {
                throw std::runtime_error("getNavMesh: Missing argument.");
            }

            auto navMeshes = m_runtime.getNavmeshes();
            auto const navMeshId = a[0].as_cstring();
            auto const navMesh = navMeshes->at(navMeshId);

            auto const regions = navMesh.regions;

            quickjs::value Object = ctx->get_global_object().get_property("Object");

            quickjs::value Array = ctx->get_global_object().get_property("Array");
            quickjs::value pathArray = Array.call_member("from", 0);

            for (const auto &region : regions) {
                quickjs::value regionObj = Object.call_member("create", quickjs::value::null(*ctx));
                quickjs::value verticesArray = Array.call_member("from", 0);

                for (const auto &vertex : region->vertices) {
                    quickjs::value vertexObj = Object.call_member("create", quickjs::value::null(*ctx));

                    vertexObj.set_property("x", static_cast<double>(vertex.x));
                    vertexObj.set_property("y", static_cast<double>(vertex.y));

                    verticesArray.call_member("push", vertexObj);
                }

                regionObj.set_property("vertices", verticesArray);

                pathArray.call_member("push", regionObj);
            }

            return pathArray;
        });
    }

    /**
     * @namespace Sound
     *
     * @description Provides sound-related functionalities.
     */
    void JSBindings::bindSoundMethods(quickjs::value &global) {
        createNamespace(global, "Sound");

        bindLoadSound(global);
        bindPlaySound(global);
        bindStopSound(global);

        bindSetSoundVolume(global);
    }

    /**
     * @function loadSound
     *
     * @param {string} path - The path to the sound file.
     *
     * @description Loads a sound file.
     *
     * @returns {number} - The ID of the loaded sound.
     *
     * @example const sound = Sound.loadSound("assets/sounds/jump.wav"); // Loads the sound file.
     */
    void JSBindings::bindLoadSound(quickjs::value &global) {
        auto sound = global.get_property("Sound");

        sound.set_property("loadSound", [this](const quickjs::args &a) -> quickjs::value {
            std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("loadSound: Missing argument.");
            }

            std::string path = a[0].as_cstring().c_str();
            auto soundId = m_runtime.getAudio()->LoadSound(path);

            return {*ctx, static_cast<double>(soundId)};
        });
    }

    /**
     * @function playSound
     *
     * @param {number} soundId - The ID of the sound to play.
     *
     * @description Plays a sound.
     *
     * @example Sound.playSound(soundId); // Plays the sound with the given ID.
     */
    void JSBindings::bindPlaySound(quickjs::value &global) {
        auto sound = global.get_property("Sound");

        sound.set_property("playSound", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("playSound: Missing argument.");
            }

            auto soundId = a[0].as_uint32();
            m_runtime.getAudio()->PlaySound(soundId);
        });
    }

    /**
     * @function stopSound
     *
     * @param {number} soundId - The ID of the sound to stop.
     *
     * @description Stops a sound.
     *
     * @example Sound.stopSound(soundId); // Stops the sound with the given ID.
     */
    void JSBindings::bindStopSound(quickjs::value &global) {
        auto sound = global.get_property("Sound");

        sound.set_property("stopSound", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 1) {
                throw std::runtime_error("stopSound: Missing argument.");
            }

            auto soundId = a[0].as_uint32();
            m_runtime.getAudio()->StopSound(soundId);
        });
    }

    /**
     * @function setSoundVolume
     *
     * @param {number} soundId - The ID of the sound to set the volume for.
     * @param {number} volume - The volume to set (0.0 to 1.0).
     *
     * @description Sets the volume of a sound.
     *
     * @example Sound.setSoundVolume(soundId, 0.5); // Sets the volume of the sound with the given ID to 0.5.
     */
    void JSBindings::bindSetSoundVolume(quickjs::value &global) {
        auto sound = global.get_property("Sound");

        sound.set_property("setSoundVolume", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            if (argsCount < 2) {
                throw std::runtime_error("setSoundVolume: Missing arguments.");
            }

            auto soundId = a[0].as_uint32();
            auto volume = a[1].as_double();
            m_runtime.getAudio()->SetSoundVolume(soundId, volume);
        });
    }

} // runtime
// blipcade
