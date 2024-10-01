import {state} from "./state/state";
import {PICO8_COLORS} from "../celeste/colors";
import {Entities, MoveSystem, ParticlesSystem} from "./entities";

function init() {
    Graphics.setTransparentColor(255);

    state.SPRITES = {
        CURSOR: 0
    }

    state.frameCount = 0;
    state.lastFPSUpdate = Date.now();
    state.currentFPS = 0;

    state.keyStates = {};
    state.mouseButtonStates = {};

    // state.particlesSystem = new ParticlesSystem();

    state.entities = new Entities();
    state.moveSystem = new MoveSystem();
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

    // X key
    if (Input.isKeyPressed(32)) {
        if (state.keyStates[32] !== 'held' && state.keyStates[32] !== 'pressed') {
            state.keyStates[32] = 'pressed';
        }
    } else {
        if (state.keyStates[32] === 'held' || state.keyStates[32] === 'pressed') {
            state.keyStates[32] = 'released';
        }
    }

    // Left key
    if (Input.isKeyPressed(1)) {
        if (state.keyStates[1] !== 'held' && state.keyStates[1] !== 'pressed') {
            state.keyStates[1] = 'pressed';
        }
    } else {
        if (state.keyStates[1] === 'held' || state.keyStates[1] === 'pressed') {
            state.keyStates[1] = 'released';
        }
    }

    // Right key
    if (Input.isKeyPressed(2)) {
        if (state.keyStates[2] !== 'held' && state.keyStates[2] !== 'pressed') {
            state.keyStates[2] = 'pressed';
        }
    } else {
        if (state.keyStates[2] === 'held' || state.keyStates[2] === 'pressed') {
            state.keyStates[2] = 'released';
        }
    }
    
    // Up key
    if (Input.isKeyPressed(4)) {
        if (state.keyStates[4] !== 'held' && state.keyStates[4] !== 'pressed') {
            state.keyStates[4] = 'pressed';
        }
    } else {
        if (state.keyStates[4] === 'held' || state.keyStates[4] === 'pressed') {
            state.keyStates[4] = 'released';
        }
    }

    // Down key
    if (Input.isKeyPressed(8)) {
        if (state.keyStates[8] !== 'held' && state.keyStates[8] !== 'pressed') {
            state.keyStates[8] = 'pressed';
        }
    } else {
        if (state.keyStates[8] === 'held' || state.keyStates[8] === 'pressed') {
            state.keyStates[8] = 'released';
        }
    }

    Object.keys(state.mouseButtonStates).forEach(button => {
        if (state.mouseButtonStates[button] === 'pressed') {
            state.mouseButtonStates[button] = 'held';
        } else if (state.mouseButtonStates[button] === 'released') {
            state.mouseButtonStates[button] = 'up';
        }
    });

    if (Input.isMouseButtonPressed(0)) {
        if (state.mouseButtonStates[0] !== 'held' && state.mouseButtonStates[0] !== 'pressed') {
            state.mouseButtonStates[0] = 'pressed';
        }
    } else {
        if (state.mouseButtonStates[0] === 'held' || state.mouseButtonStates[0] === 'pressed') {
            state.mouseButtonStates[0] = 'released';
        }
    }

    if (state.mouseButtonStates[0] === 'pressed') {
        const coords = Input.getMousePos();
        log(`Mouse clicked at ${coords.x}, ${coords.y}`);
    }

    state.moveSystem.update(state.FRAME_TIME); // ms
    state.entities.update(state.FRAME_TIME);

    // update code
    // state.particlesSystem.update(elapsed);
    // instead of elapsed, we will use fixed delta time:
    // state.particlesSystem.update(state.FRAME_TIME);
}

function draw() {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    Graphics.fillScreen(0);

    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        state.color = (state.color + 1) % 255;
    }

    state.entities.draw();

    const mousePos = Input.getMousePos();

    Graphics.drawSprite(mousePos.x, mousePos.y, state.SPRITES.CURSOR, false, false);

    drawFPSCounter();
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