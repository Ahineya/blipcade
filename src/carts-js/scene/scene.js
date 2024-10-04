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

    // state.particlesSystem = new ParticlesSystem();

    state.entities = new Entities();

    // name = "Particles Emitter",
    //     position = { x: 0, y: 0 },
    //     emissionRate = 2000,
    //     particleLifetime = 0.5,
    //     initialVelocity = { x: 0, y: 2 },
    //     velocityVariation = 20,
    //     particleClass = Particle

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

    // TODO: THis bullshit should be handled by the engine

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
