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
import {levelSystem} from "./systems/level.system";
import {particlesSystem} from "./systems/particles.system";

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
    levelSystem.loadLevel("level1");

    // TODO: This is not ideal. We should have a separate system for particle emitters
    // const createMiasmaEmitter = (position) => {
    //     return new ParticlesEmitter("Miasma Emitter", position, 5, 10, {x: 0, y: 2}, 2, MiasmaParticle);
    // }

    // particlesSystem.addEmitter(createMiasmaEmitter({x: 261, y: 164}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 302, y: 171}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 239, y: 202}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 66, y: 155}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 91, y: 201}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 66, y: 215}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 16, y: 145}));
    // particlesSystem.addEmitter(createMiasmaEmitter({x: 320 / 2, y: 200}));
    // particlesSystem.addEmitter(
    //     new ParticlesEmitter("Left fire", {
    //         x: 60,
    //         y: 60
    //     }, 50, 2, {x: 1, y: 5}, 0, Particle)
    // );

    state.systems = [
        levelSystem,
        drawSystem,
        movementSystem,
        animationSystem,

        particlesSystem,
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

    if (state.keyStates[keyCode] === 'pressed' && keyCode === 32) {
        const levelComponent = ECS.getComponent(levelSystem.levelControllerEntity, "LevelController");
        if (levelComponent.currentLevel === "level1") {
            levelComponent.loadLevel = "level2";
        } else {
            levelComponent.loadLevel = "level1";
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

}
