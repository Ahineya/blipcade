// import {Mouse} from "../framework/mouse";
import {state} from "./state/state";
import {PICO8_COLORS} from "../celeste/colors";
import {Background} from "./backgrounds";

function init() {
    Graphics.setTransparentColor(255);

    state.SPRITES = {
        CURSOR: 0
    }

    state.color = 0;

    state.frameCount = 0;
    state.lastFPSUpdate = Date.now();
    state.currentFPS = 0;

    const background = new Background();
    background.load(0);
    state.background = background;

    state.keyStates = {};
}

function update() {
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

    Object.keys(state.keyStates).forEach(key => {
        if (state.keyStates[key] === 'pressed') {
            state.keyStates[key] = 'held';
        } else if (state.keyStates[key] === 'released') {
            state.keyStates[key] = 'up';
        }
    });

    if (Input.isKeyPressed(32)) {
        if (state.keyStates[32] !== 'held' && state.keyStates[32] !== 'pressed') {
            state.keyStates[32] = 'pressed';
            state.background.next();
        }
    } else {
        if (state.keyStates[32] === 'held' || state.keyStates[32] === 'pressed') {
            state.keyStates[32] = 'released';
        }
    }

    state.background.update();

    state.keyJustPressed = false;
}

function draw() {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    Graphics.fillScreen(0);

    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        state.color = (state.color + 1) % 255;
    }

    state.background.draw();

    text(state.background.currentName || "", 1, 1, 0xfe);

    drawFPSCounter();

    // drawSprite(Mouse.coords.x, Mouse.coords.y, state.SPRITES.CURSOR, false, false);
}

globalThis.draw = draw;
globalThis.update = update;
globalThis.init = init;

function drawFPSCounter() {
    const fpsText = `fps: ${state.currentFPS}`;
    text(fpsText,1, 2,  PICO8_COLORS[0]);
    text(fpsText, 3, 2,  PICO8_COLORS[0]);
    text(fpsText,2, 3,  PICO8_COLORS[0]);
    text(fpsText,2, 1,  PICO8_COLORS[0]);
    text(fpsText,2, 2,  PICO8_COLORS[7]);
}