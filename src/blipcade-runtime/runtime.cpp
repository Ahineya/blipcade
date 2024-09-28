//
// Created by Pavlo Yevsehnieiev
//

#include "runtime.h"

#include <canvas.h>
#include <converters.h>
#include <iostream>
#include <quickjs.hpp>

#include "JsBindings.h"
#include "keystate.h"

// Header for reference:

/*
// Created by Pavlo Yevsehnieiev
//

#define RUNTIME_H
#include <memory>
#include <optional>
#include <string>


namespace blipcade::runtime {

 * So Runtime should be a singleton class that will be responsible for the main loop of the engine.
 *
 * It should know about:
 * - canvas
 * - quickjs runtime
 * - input
 * - audio


    // class JsRuntime;
    // class Context;
    class Canvas;
    class Palette685;
    class Spritesheet;
    // class Maps;
    // class KeyState;
    // class MouseState;
    class Font;
    // class RuntimeControl;
    // class AudioEngine;
    // class RuntimeCartList;

    class Runtime {
    public:
        Runtime(
            std::shared_ptr<Canvas> canvas,
            std::shared_ptr<Spritesheet> spritesheet,
            // std::shared_ptr<Maps> maps,
            // std::shared_ptr<KeyState> key_state,
            // std::shared_ptr<MouseState> mouse_state,
            // std::shared_ptr<RuntimeControl> runtime_state,
            std::shared_ptr<Font> font
            // #if !defined(__EMSCRIPTEN__)
            // std::shared_ptr<AudioEngine> audio_engine,
            // #else
            // std::shared_ptr<AudioEngine> audio_engine,
            // #endif
            // RuntimeCartList runtime_cart_list
        );

        void init();
        void update();
        void draw();

        // Let's make it possible to get a pointer to the canvas
        [[nodiscard]] std::shared_ptr<Canvas> getCanvas() const;

    private:
        // std::unique_ptr<JsRuntime> js_runtime;
        // std::shared_ptr<Context> context;
        std::shared_ptr<Canvas> canvas;
        std::shared_ptr<Spritesheet> spritesheet;
        // std::shared_ptr<Maps> maps;
        std::shared_ptr<std::string> code;
        // std::shared_ptr<KeyState> key_flags;
        // std::shared_ptr<MouseState> mouse_state;
        std::shared_ptr<Font> font;
        // std::shared_ptr<RuntimeControl> runtime_state;

// #if !defined(__EMSCRIPTEN__)
        // std::shared_ptr<AudioEngine> audio_engine;
// #else
        // std::shared_ptr<AudioEngine> audio_engine;
// #endif

        // RuntimeCartList runtime_cart_list;

        std::string cart_to_load;

        std::shared_ptr<std::optional<std::pair<uint32_t, uint32_t>>> resize_request;

        void register_global_functions();
    };

} // runtime
// blipcade

*/

namespace blipcade::runtime {
    Runtime::Runtime(): canvas(nullptr), key_flags(std::make_shared<Keystate>()), font(nullptr),
                        js_bindings(std::make_unique<JSBindings>(*this)) {
        canvas = std::make_shared<graphics::Canvas>(128, 128);
        std::cout << "Runtime constructed at " << this << " with Canvas at " << canvas.get()
                << " and pixels at " << canvas->getPixels().data() << std::endl;
        spritesheet = std::make_shared<graphics::Spritesheet>(0, 0);

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

    void Runtime::init() {
        js_runtime = std::make_unique<quickjs::runtime>();
        context = std::make_shared<quickjs::context>(js_runtime->new_context());

        quickjs::value global = context->get_global_object();

        js_bindings->registerAll(global);

        auto code = R"javascript(
class Mouse_ {
    #listeners = new Map();
    #state = {
        x: 0,
        y: 0,
        left: false,
        right: false
    };
    constructor(){
        this.#listeners.set('move', []);
        this.#listeners.set('leftDown', []);
        this.#listeners.set('leftUp', []);
    }
    get coords() {
        return {
            x: this.#state.x,
            y: this.#state.y
        };
    }
    addEventListener(event, callback) {
        this.#listeners.set(event, [
            ...this.#listeners.get(event),
            callback
        ]);
        return ()=>{
            this.removeEventListener(event, callback);
        };
    }
    removeEventListener(event, callback) {
        this.#listeners.set(event, this.#listeners.get(event).filter((listener)=>listener !== callback));
    }
    #emit(event, data) {
        const listeners = this.#listeners.get(event);
        if (listeners) listeners.forEach((listener)=>listener(data));
    }
    mouseUpdate(mouseState) {
        const x = mouseState.x;
        const y = mouseState.y;
        const left = mouseState.left;
        const right = mouseState.right;
        if (x !== this.#state.x || y !== this.#state.y) {
            this.#state.x = x;
            this.#state.y = y;
            this.#emit('move', {
                x,
                y
            });
        }
        if (left !== this.#state.left) {
            this.#state.left = left;
            this.#emit(left ? 'leftDown' : 'leftUp', {
                x,
                y
            });
        }
        if (right !== this.#state.right) {
            this.#state.right = right;
            this.#emit(right ? 'rightDown' : 'rightUp', {
                x,
                y
            });
        }
    }
}
const Mouse = new Mouse_();
const APP_STATE = {
    LOADING: 'loading',
    LOADED: 'loaded',
    DEVELOPMENT: 'development'
};
class State {
    appState = APP_STATE.LOADING;
    FONT_SIZE = {
        width: 5,
        height: 7
    };
    FPS = 30;
    FRAME_TIME = 1000 / this.FPS;
    lastUpdateTime = Date.now();
    lastDrawTime = Date.now();
    frames = 0;
    seconds = 0;
    objects = [];
    particleManager = null;
    constructor(){}
}
const state = new State();
const PICO8_COLORS = {
    0: 0x00,
    1: 0x2E,
    2: 0x56,
    3: 0x15,
    4: 0x83,
    5: 0x5B,
    6: 0xBC,
    7: 0xEF,
    8: 0xC9,
    9: 0xE1,
    10: 0xEC,
    11: 0x1F,
    12: 0x45,
    13: 0x89,
    14: 0xDA,
    15: 0xE9
};
const gridcolor = 12;
class Background {
    constructor(){
        this.t = 0;
        this.list = [
            "loopy",
            "starfield",
            "grid",
            "balls",
            "flurry",
            "ballspiral",
            "flight",
            "circles",
            "rainbow",
            "spiral",
            "matrix"
        ];
        this.rand = [];
        this.rand2 = [];
        this.current = null;
        this.currentName = null;
        this.star_sx = 64;
        this.star_sy = 64;
        this.star_cutoff = 12;
        this.stars = [];
        this.spots = [];
        this.backgroundTypes = {
            loopy: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[0],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[1],
                load: ()=>{
                    this.rand = Array.from({
                        length: 16384
                    }, ()=>Math.random() * 16);
                },
                update: ()=>{
                    //this.rand.forEach((p, i)=>{});
                },
                draw: ()=>{
                    this.rand.forEach((p, i)=>{
                        const x = i % 128;
                        const y = Math.floor(i / 128);
                        Graphics.putPixel(x, y, PICO8_COLORS[Math.floor(p)]);
                    });
                }
            },
            flurry: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[0],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[1],
                load: ()=>{
                    this.rand = [];
                    this.rand2 = [];
                    for(let n = 0; n < 256; n++){
                        this.rand.push(Math.random() * 128);
                        this.rand2.push(Math.random() * 128);
                    }
                },
                draw: ()=>{
                    const p = [
                        1,
                        1,
                        13,
                        13,
                        2,
                        14,
                        6
                    ];
                    for(let i = 0; i < 256; i++){
                        const z = Math.floor(i / 40) + 1;
                        Graphics.drawFilledCircle((this.rand[i] - this.t * z * z) % 140 - 8, this.rand2[i], z, PICO8_COLORS[p[z]] || 0xfe);
                    }
                    this.t += 0.05;
                }
            },
            balls: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[2],
                hfg: PICO8_COLORS[1],
                pbg: PICO8_COLORS[8],
                load: ()=>{
                    for(let n = 0; n < 500; n++)this.rand.push(Math.random() * 9);
                },
                draw: ()=>{
                    for(let i = 0; i < 500; i++){
                        let p = 9 - i / 50;
                        let j = (p + Math.sin(this.t) - this.t * 9) % 11 + 0.1;
                        let k = this.rand[i] - 5 + Math.cos(this.t);
                        if (Math.floor(p) === 7) p = 14;
                        Graphics.drawFilledCircle(k / j * 50 + 64, 80 / j - 20, 9 / j, PICO8_COLORS[Math.floor(p)] || 0xfe);
                    }
                    this.t += 0.01;
                }
            },
            rainbow: {
                color: PICO8_COLORS[2],
                mfg: PICO8_COLORS[1],
                mbg: PICO8_COLORS[7],
                hfg: PICO8_COLORS[2],
                pbg: PICO8_COLORS[2],
                draw: ()=>{
                    const m = 12;
                    this.t += 0.05;
                    for(let n = 0; n < m * m; n++){
                        const x = n % m;
                        const y = Math.floor(n / m);
                        Graphics.drawFilledCircle(x * 12, y * 12, 12 + 4 * Math.cos(this.t + y / 5) + 4 * Math.sin(this.t + x / 7), PICO8_COLORS[8 + (x + y) % 8] || 0xfe);
                    }
                }
            },
            spiral: {
                color: PICO8_COLORS[1],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[5],
                hfg: PICO8_COLORS[6],
                pbg: PICO8_COLORS[0],
                draw: ()=>{
                    const s = 64;
                    this.t += 0.0001;
                    let x = s, y = s;
                    for(let i = 1; i <= 350; i++){
                        const j = i * this.t;
                        const u = x, v = y;
                        x = x + j * Math.sin(j);
                        y = y + j * Math.cos(j);
                        Graphics.drawLine(u, v, x, y, PICO8_COLORS[7 + Math.floor(i / 60)] || 0xfe);
                    }
                }
            },
            circles: {
                color: PICO8_COLORS[2],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[0],
                draw: ()=>{
                    this.t += 0.01;
                    for(let k = 0; k <= 16; k++)for(let n = 1; n <= 9; n++){
                        const h = k / 16 + this.t;
                        Graphics.drawCircle(64 + Math.cos(h + this.t / 3) * n * 8, 64 + Math.sin(h) * (n * n + Math.cos(this.t) * 16), n, PICO8_COLORS[11 - Math.floor(n / 3)] || 0xfe);
                    }
                }
            },
            ballspiral: {
                color: PICO8_COLORS[9],
                mfg: PICO8_COLORS[0],
                mbg: PICO8_COLORS[7],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[4],
                draw: ()=>{
                    let x = 64, y = 64, r = 1, a = 0;
                    for(let i = 0; i <= 150; i++){
                        Graphics.drawFilledCircle(x, y, r / 2, PICO8_COLORS[6 + i % 3] || 0xfe);
                        x += Math.cos(a) * r;
                        y += Math.sin(a) * r;
                        r += 1 / 4;
                        a += this.t / 5;
                    }
                    this.t += 0.002;
                }
            },
            grid: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[2],
                hfg: PICO8_COLORS[13],
                pbg: PICO8_COLORS[2],
                draw: ()=>{
                    const w = 127;
                    const n = 15;
                    this.t += 0.75;
                    for(let i = 0; i <= n; i++){
                        const z = i * n + this.t % n;
                        const y = w * n / z + 32;
                        Graphics.drawLine(0, y, w, y, PICO8_COLORS[gridcolor]);
                        const v = i + this.t % n / n - n / 2;
                        Graphics.drawLine(v * 9 + 64, 40, v * 60 + 64, w, PICO8_COLORS[gridcolor] || 0xfe);
                    }
                }
            },
            flight: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[5],
                draw: ()=>{
                    const n = 32;
                    const sin_t_over_300 = Math.sin(this.t / 300);
                    const t_mod_n = this.t % n;
                    const c = sin_t_over_300;
                    const sin_t_over_9 = c / 9;
                    const sin_t_over_3 = c / 3;
                    const const1 = sin_t_over_3 + 1;
                    const make = (y, color)=>{
                        for(let i = 0; i <= n; i++){
                            const z = (i * n + t_mod_n) / 1500;
                            for(let j = -n; j <= n; j++){
                                const x = (j * 0.7 + c) / z + y;
                                const y_pos = (-j * sin_t_over_9 + const1) / z + y;
                                Graphics.putPixel(x, y_pos, PICO8_COLORS[color] || 0xfe);
                            }
                        }
                    };
                    this.t -= 2;
                    make(5, 8);
                    make(40, 9);
                    make(70, 10);
                }
            },
            matrix: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[3],
                hfg: PICO8_COLORS[11],
                pbg: PICO8_COLORS[3],
                draw: () => {
                    for(let j = 3; j <= 99; j++){
                        let c = 11;
                        const x = j * 593;
                        for(let y = 1; y <= x % c; y++){
                            const n = Math.pow(j, y) % 7 + 1;
                            const __char = "5&y$%z?$*".charAt(n - 1);
                            text(__char, x % 126, (this.t + x) * j / 8 % 256 - y * 6, PICO8_COLORS[c] || 0xfe);
                            c = 3;
                        }
                    }
                    this.t += 0.7;
                }
            },
            starfield: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[5],
                load: ()=>{
                    this.stars = [];
                    this.spots = [];
                },
                update: ()=>{
                    for(let i = 0; i < 2; i++)if (this.stars.length < 100) {
                        let star = {
                            x: this.star_sx,
                            y: this.star_sy,
                            d: Math.random(),
                            spd: i + 1
                        };
                        let center_x = 64;
                        let center_y = 64;
                        let distance = Math.sqrt(Math.pow(star.x - center_x, 2) + Math.pow(star.y - center_y, 2));
                        while(distance < 32){
                            star.x = Math.random() * 128;
                            star.y = Math.random() * 128;
                            distance = Math.sqrt(Math.pow(star.x - center_x, 2) + Math.pow(star.y - center_y, 2));
                        }
                        this.stars.push({
                            x: this.star_sx,
                            y: this.star_sy,
                            d: Math.random(),
                            spd: i + 1
                        });
                    }
                    this.stars = this.stars.filter((s)=>{
                        s.d += 1;
                        s.x += Math.cos(s.d * Math.PI * 2) * s.spd;
                        s.y += Math.sin(s.d * Math.PI * 2) * s.spd;
                        if (s.d > this.star_cutoff - s.spd) for(let i = 1; i <= Math.floor(s.spd); i++)this.spots.push({
                            x: s.x - Math.cos(s.d * Math.PI * 2) * s.spd / i,
                            y: s.y - Math.sin(s.d * Math.PI * 2) * s.spd / i,
                            d: s.spd * 2
                        });
                        return !(s.x < -32 || s.x > 128 || s.y < -16 || s.y > 128);
                    });
                    this.spots = this.spots.filter((s)=>{
                        s.d -= 1;
                        return s.d >= 0;
                    });
                },
                draw: ()=>{
                    this.stars.forEach((s)=>{
                        Graphics.putPixel(s.x, s.y, PICO8_COLORS[7]);
                    });
                    this.spots.forEach((s)=>{
                        Graphics.putPixel(s.x, s.y, PICO8_COLORS[5]);
                    });
                }
            }
        };
    }
    createStar() {
        const angle = Math.random() * Math.PI * 2;
        return {
            x: this.star_sx,
            y: this.star_sy,
            d: angle,
            spd: Math.random() * 0.5 + 0.1
        };
    }
    load(stageId) {
        let bgp = this.list[stageId];
        this.current = this.backgroundTypes[bgp];
        this.currentName = bgp;
        this.rand = [];
        this.rand2 = [];
        this.t = 0;
        if (this.current && this.current.load) this.current.load();
    }
    next() {
        this.load((this.list.indexOf(this.currentName) + 1) % this.list.length);
    }
    update() {
        this.t += 0.01;
        if (this.current && this.current.update) this.current.update();
    }
    draw() {
        if (this.current && this.current.draw) this.current.draw();
    }
}

function init(){
    setTransparentColor(255);
    state.SPRITES = {
        CURSOR: 0
    };
    state.color = 0;
    state.frameCount = 0;
    state.lastFPSUpdate = Date.now();
    state.currentFPS = 0;
    const background1 = new Background();
    background1.load(0);
    state.background = background1;
    state.keyStates = {};
}
function update() {
    //Mouse.mouseUpdate(getMouseState());
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastUpdateTime;
    if (elapsed >= state.FRAME_TIME) {
        state.frameCount++;
        if (currentTime - state.lastFPSUpdate >= 1000) {
            state.currentFPS = state.frameCount;
            state.frameCount = 0;
            state.lastFPSUpdate = currentTime;
        }
    }
    Object.keys(state.keyStates).forEach((key)=>{
        if (state.keyStates[key] === 'pressed') state.keyStates[key] = 'held';
        else if (state.keyStates[key] === 'released') state.keyStates[key] = 'up';
    });
    if (Input.isKeyPressed(32)) {
        if (state.keyStates[32] !== 'held' && state.keyStates[32] !== 'pressed') {
            state.keyStates[32] = 'pressed';
            state.background.next();
        }
    } else if (state.keyStates[32] === 'held' || state.keyStates[32] === 'pressed') state.keyStates[32] = 'released';
    state.background.update();
    state.keyJustPressed = false;
};
function draw() {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    Graphics.fillScreen(0);
    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - elapsed % state.FRAME_TIME;
        state.color = (state.color + 1) % 255;
    }
    state.background.draw();
    text(state.background.currentName || "", 1, 1, 0xfe);
    //drawSprite(Mouse.coords.x, Mouse.coords.y, state.SPRITES.CURSOR, false, false);
};


        )javascript";

        evalWithStacktrace(code);
        evalWithStacktrace("init()");
    }

    void Runtime::update() const {
        evalWithStacktrace("update()");
    }

    void Runtime::draw() const {
        evalWithStacktrace("draw()");
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

} // runtime
// blipcade
