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

    // Create entities
    let playerEntity = ECS.createEntity();
    let somethingEntity = ECS.createEntity();

    log(`Entity 1: ${playerEntity}`);
    log(`Entity 2: ${somethingEntity}`);

    let positionComponent = { x: 10.0, y: 20.0 };
    let velocityComponent = { vx: 1.0, vy: 1.5 };
    let healthComponent = { hp: 100 };

    ECS.addComponent(playerEntity, "Position", positionComponent);
    ECS.addComponent(playerEntity, "Velocity", velocityComponent);
    ECS.addComponent(playerEntity, "Sprite", { spriteID: 0, flipX: false, flipY: false });

    ECS.addComponent(somethingEntity, "Position", { x: 5.0, y: 15.0 });
    ECS.addComponent(somethingEntity, "Health", healthComponent);

    ECS.forEachEntity(["Position", "Velocity"], (entityID, pos, vel) => {
        pos.x += vel.vx;
        pos.y += vel.vy;

        log(`Entity ${entityID} moved to position ${pos.x}, ${pos.y}`);
    });
    ECS.forEachEntity(["Health"], function(entityID, health) {
        log(`Entity ${entityID} has health ${health.hp}`);
    });

    // Let's create 100 entities with Position and Sprite components for testing
    for (let i = 0; i < 100; i++) {
        let entity = ECS.createEntity();
        ECS.addComponent(entity, "Position", { x: Math.random() * 256, y: Math.random() * 256 });
        ECS.addComponent(entity, "Sprite", { spriteID: 0, flipX: false, flipY: false });
    }

    // Let's make 10000 entities with just a Position component
    // let fiveThousandEntity = null;
    // for (let i = 0; i < 10000; i++) {
    //     let entity = ECS.createEntity();
    //     ECS.addComponent(entity, "Position", { x: Math.random() * 256, y: Math.random() * 256 });
    //
    //     if (i === 5000) {
    //         fiveThousandEntity = entity;
    //     }
    // }
    //
    // ECS.addComponent(fiveThousandEntity, "Sprite", { spriteID: 0, flipX: false, flipY: false });
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

    text(state.background.currentName || "", 1, 1);

    ECS.forEachEntity(["Position", "Sprite"], (entity) => {
        const pos = ECS.getComponent(entity, "Position");
        const sprite = ECS.getComponent(entity, "Sprite");
        Graphics.drawSprite(pos.x, pos.y, sprite.spriteID, 0, sprite.flipX, sprite.flipY);
    });

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