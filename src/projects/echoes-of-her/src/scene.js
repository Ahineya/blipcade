import {state} from "./state/state.js";
import {Entities} from "./entities.js";
import {movementSystem} from "./systems/movement.system.js";
import {debugSystem} from "./systems/debug.system.js";
import {drawSystem} from "./systems/draw.system.js";
import {animationSystem} from "./systems/animation.system.js";
import {lightingSystem} from "./systems/lighting.system.js";
import {soundSystem} from "./systems/sound.system.js";
import {levelSystem} from "./systems/level.system.js";
import {particlesSystem} from "./systems/particles.system.js";
import {messageSystem} from "./systems/messageSystem.js";
import {interactiveObjectsSystem} from "./systems/interactive-objects.system.js";

const CURSOR_SPRITESHEET = "res://spritesheets/cursor.json";

const mouseEventQueue = [];

export function init() {

    Graphics.setPostprocessingShader("res://shaders/pp.frag");

    Graphics.setTransparentColor(255);

    state.frameCount = 0;
    state.lastFPSUpdate = Date.now();
    state.currentFPS = 0;

    state.keyStates = {};
    state.mouseButtonStates = {
        0: 'up',
        1: 'up',
        2: 'up'
    };
    state.previousMouseButtonStates = {};

    state.entities = new Entities();
    levelSystem.loadLevel("level2");

    state.systems = {
        levelSystem,
        drawSystem,
        movementSystem,
        animationSystem,
        particlesSystem,
        interactiveObjectsSystem,
        messageSystem,
        lightingSystem,
        soundSystem,
        debugSystem
    };

    state.SPRITES = {
        CURSOR: 0
    }

    state.systems.forEach(s => {
        s.init && s.init();
    });
}

export function update() {
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

    updateKeyStates();
    updateMouseStates();
    handleMouseClicks();
    dispatchMouseEvents();

    Object.values(state.systems).forEach(s => {
        s.update && s.update(state.FRAME_TIME);
    })
}

export function draw() {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    Graphics.fillScreen(0);

    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        state.color = (state.color + 1) % 255;
    }

    Object.values(state.systems).forEach(s => {
        s.draw && s.draw(state.FRAME_TIME);
    });

    const mousePos = Input.getMousePos();

    Graphics.drawSprite(mousePos.x, mousePos.y, state.SPRITES.CURSOR, CURSOR_SPRITESHEET);
}

function updateKeyStates() {
    Object.keys(state.keyStates).forEach(key => {
        if (state.keyStates[key] === 'pressed') {
            state.keyStates[key] = 'held';
        } else if (state.keyStates[key] === 'released') {
            state.keyStates[key] = 'up';
        }
    });

    // Example for updating a specific key (e.g., Left arrow key with key code 1)
    updateSingleKeyState(1 << 0, Input.isKeyPressed(1));
    updateSingleKeyState(1 << 1, Input.isKeyPressed(2));
    updateSingleKeyState(1 << 2, Input.isKeyPressed(4));
    updateSingleKeyState(1 << 3, Input.isKeyPressed(8));
    updateSingleKeyState(1 << 4, Input.isKeyPressed(16));
    updateSingleKeyState(1 << 5, Input.isKeyPressed(32));
}

function updateSingleKeyState(keyCode, isPressed) {
    if (isPressed) {
        if (state.keyStates[keyCode] !== 'held' && state.keyStates[keyCode] !== 'pressed') {
            state.keyStates[keyCode] = 'pressed';
        }
    } else {
        if (state.keyStates[keyCode] === 'held' || state.keyStates[keyCode] === 'pressed') {
            state.keyStates[keyCode] = 'released';
        }
    }

    if (state.keyStates[keyCode] === 'pressed' && keyCode === 32) {
        const levelComponent = ECS.getComponent(levelSystem.levelControllerEntity, "LevelController");
        if (levelComponent.currentLevel === "level1") {
            levelComponent.loadLevel = "level2";
        } else if (levelComponent.currentLevel === "level2") {
            levelComponent.loadLevel = "level3";
        } else if (levelComponent.currentLevel === "level3") {
            levelComponent.loadLevel = "level4";
        }
        else {
            levelComponent.loadLevel = "level1";
        }
    }
}

// function updateMouseStates() {
//     Object.keys(state.mouseButtonStates).forEach(button => {
//         if (state.mouseButtonStates[button] === 'pressed') {
//             state.mouseButtonStates[button] = 'held';
//         } else if (state.mouseButtonStates[button] === 'released') {
//             state.mouseButtonStates[button] = 'up';
//         }
//     });
// }
//
// function handleMouseClicks() {
//     // Handle left mouse button (button index 0)
//     const button = 0;
//     const isPressed = Input.isMouseButtonPressed(button);
//     if (isPressed) {
//         if (state.mouseButtonStates[button] !== 'held' && state.mouseButtonStates[button] !== 'pressed') {
//             state.mouseButtonStates[button] = 'pressed';
//         }
//     } else {
//         if (state.mouseButtonStates[button] === 'held' || state.mouseButtonStates[button] === 'pressed') {
//             state.mouseButtonStates[button] = 'released';
//         }
//     }
// }

function captureMouseEvents() {
    Object.keys(state.mouseButtonStates).forEach(button => {
        const currentState = state.mouseButtonStates[button];
        if (currentState === 'pressed') {
            mouseEventQueue.push({ type: 'mouseDown', button, position: Input.getMousePos() });
        } else if (currentState === 'released') {
            mouseEventQueue.push({ type: 'mouseUp', button, position: Input.getMousePos() });
        }
    });
}

function updateMouseStates() {
    Object.keys(state.mouseButtonStates).forEach(button => {
        if (state.mouseButtonStates[button] === 'pressed') {
            state.mouseButtonStates[button] = 'held';
        } else if (state.mouseButtonStates[button] === 'released') {
            state.mouseButtonStates[button] = 'up';
        }
    });
}

function handleMouseClicks() {
    // Handle mouse state transitions and capture events
    // This assumes Input.isMouseButtonPressed(button) is updated elsewhere
    Object.keys(state.mouseButtonStates).forEach(button => {
        const isPressed = Input.isMouseButtonPressed(button);
        if (isPressed) {
            if (state.mouseButtonStates[button] !== 'held' && state.mouseButtonStates[button] !== 'pressed') {
                state.mouseButtonStates[button] = 'pressed';
            }
        } else {
            if (state.mouseButtonStates[button] === 'held' || state.mouseButtonStates[button] === 'pressed') {
                state.mouseButtonStates[button] = 'released';
            }
        }
    });

    captureMouseEvents();
}

function dispatchMouseEvents() {
    while (mouseEventQueue.length > 0) {
        const event = mouseEventQueue.shift();
        let eventHandled = false;

        log(`Mouse event: ${event.type} button ${event.button} at ${event.position.x}, ${event.position.y}`);

        for (const [name, system] of Object.entries(state.systems)) {
            log(`Dispatching event to system: ${name}`);
            if (system.handleMouseEvent) {
                if (system.handleMouseEvent(event)) {
                    eventHandled = true;
                    break; // Stop propagation if the event is handled
                }
            }
        }

        if (!eventHandled) {
            // Default handling or logging
        }
    }
}

