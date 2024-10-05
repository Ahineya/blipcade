import {state} from "./state/state";
import {PICO8_COLORS} from "../celeste/colors";
import {Entities} from "./entities";
import {movementSystem} from "./systems/movement.system";
import {debugSystem} from "./systems/debug.system";
import {drawSystem} from "./systems/draw.system";
import {animationSystem} from "./systems/animation.system";
import {lightingSystem} from "./systems/lighting.system";
import {soundSystem} from "./systems/sound.system";
import {MiasmaParticle, Particle, ParticlesEmitter} from "./particles";

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

    state.entities = new Entities();

    // TODO: This is not ideal. We should have a separate system for particle emitters
    const createMiasmaEmitter = (position) => {
        return new ParticlesEmitter("Miasma Emitter", position, 5, 10, { x: 0, y: 2 }, 2, MiasmaParticle);
    }

    state.systems = [
        drawSystem,
        movementSystem,
        animationSystem,
        new ParticlesEmitter("Left fire", {
            x: 60,
            y: 60
        }, 50, 2, { x: 1, y: 5 }, 0, Particle),
        // new ParticlesEmitter("Particles Emitter 2", {
        //     x: 250,
        //     y: 60
        // }, 2000, 0.5, { x: 0, y: 20 }, 2, Particle),
        createMiasmaEmitter({x: 261, y: 164}),
        createMiasmaEmitter({x: 302, y: 171}),
        createMiasmaEmitter({x: 239, y: 202}),
        createMiasmaEmitter({x: 66, y: 155}),
        createMiasmaEmitter({x: 91, y: 201}),
        createMiasmaEmitter({x: 66, y: 215}),
        createMiasmaEmitter({x: 16, y: 145}),
        createMiasmaEmitter({x: 320/2, y: 200}),

        lightingSystem,
        soundSystem,
        debugSystem
    ];

    state.systems.forEach(s => {
        s.init && s.init();
    })
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

    updateKeyStates();
    updateMouseStates();
    handleMouseClicks();

    if (state.mouseButtonStates[0] === 'pressed') {
        // const coords = Input.getMousePos();
        //log(`Mouse clicked at ${coords.x}, ${coords.y}`);
        // const path = Pathfinding.findPath(0, 0, coords.x, coords.y, 0);
        //log(`Path length: ${path.length}`);

        // Let's draw the path with lines
        // for (let i = 0; i < path.length - 1; i++) {
        //     const p1 = path[i];
        //     const p2 = path[i + 1];
        //     Graphics.drawLine(p1.x, p1.y, p2.x, p2.y, 0xfe);
        //     log(`Drawing line from ${p1.x}, ${p1.y} to ${p2.x}, ${p2.y}`);
        // }
    }

    state.systems.forEach(s => {
        s.update && s.update(state.FRAME_TIME);
    })
}

function draw() {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    Graphics.fillScreen(0);

    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        state.color = (state.color + 1) % 255;
    }

    state.systems.forEach(s => {
        s.draw && s.draw(state.FRAME_TIME);
    })

    const mousePos = Input.getMousePos();

    Graphics.drawSprite(mousePos.x, mousePos.y, state.SPRITES.CURSOR, false, false);
}

globalThis.draw = draw;
globalThis.update = update;
globalThis.init = init;

function updateKeyStates() {
    Object.keys(state.keyStates).forEach(key => {
        if (state.keyStates[key] === 'pressed') {
            state.keyStates[key] = 'held';
        } else if (state.keyStates[key] === 'released') {
            state.keyStates[key] = 'up';
        }
    });

    // Example for updating a specific key (e.g., Left arrow key with key code 1)
    updateSingleKeyState(1, Input.isKeyPressed(1));
    updateSingleKeyState(2, Input.isKeyPressed(2));
    updateSingleKeyState(4, Input.isKeyPressed(4));
    updateSingleKeyState(8, Input.isKeyPressed(8));
    updateSingleKeyState(32, Input.isKeyPressed(32)); // Space key
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
    // Handle left mouse button (button index 0)
    const button = 0;
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

    // If the left mouse button was just pressed, set the destination
    if (state.mouseButtonStates[button] === 'pressed') {
        const coords = Input.getMousePos();
        log(`Mouse clicked at ${coords.x}, ${coords.y}`);

        // Assuming player entity has a unique identifier, e.g., "Player"

        ECS.forEachEntity(["Player"], (playerEntity, player) => {
            if (playerEntity) {
                const currentPosition = player.position;

                log(`Current position: ${currentPosition.x}, ${currentPosition.y}`);

                // Calculate the path from current position to clicked position
                const path = Pathfinding.findPath(
                    Math.round(currentPosition.x),
                    Math.round(currentPosition.y),
                    Math.round(coords.x),
                    Math.round(coords.y),
                    0 // Assuming '0' is a parameter for the pathfinding algorithm
                );

                log(`Path length: ${path.length}`);

                if (path.length > 0) {
                    player.path = path; // Store the path in the player's component
                    player.currentPathIndex = 0; // Reset the current path index
                    log(`Path length: ${path.length}`);

                    // Optionally, draw the path for debugging
                    // drawPath(path);
                } else {
                    log("No path found to the destination.");
                }
            }
        });
    }
}

function drawPath(path) {
    for (let i = 0; i < path.length - 1; i++) {
        const p1 = path[i];
        const p2 = path[i + 1];
        Graphics.drawLine(p1.x, p1.y, p2.x, p2.y, 0xfe);
        log(`Drawing line from ${p1.x}, ${p1.y} to ${p2.x}, ${p2.y}`);
    }
}