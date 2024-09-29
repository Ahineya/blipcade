import {gameManager} from "./game-manager";
import {PICO8_COLORS} from "./colors";
import {state} from "./state";

global.init = () => {
    setClippingRect(0, 0, 128, 128);

    global.FPS = 30;
    global.FRAME_TIME = 1000 / global.FPS;
    global.lastUpdateTime = Date.now();
    global.lastDrawTime = Date.now();
    setTransparentColor(PICO8_COLORS[0]);

    global._drawSprite = global.drawSprite;

    global.drawSprite = (spr, x, y, w, h, flipX, flipY) => {
        global._drawSprite(x, y, spr, w, h, flipX, flipY);
    }

    global.frameCount = 0;
    global.lastFPSUpdate = Date.now();
    global.currentFPS = 0;

    // global.DEBUG = true;
};

global.update = () => {
    const currentTime = Date.now();
    const elapsed = currentTime - global.lastUpdateTime;

    if (elapsed >= global.FRAME_TIME) {
        global.lastUpdateTime = currentTime - (elapsed % global.FRAME_TIME);

        // Update FPS counter
        global.frameCount++;
        if (currentTime - global.lastFPSUpdate >= 1000) {
            global.currentFPS = global.frameCount;
            global.frameCount = 0;
            global.lastFPSUpdate = currentTime;
        }
    }

    gameManager.update();
};

global.draw = () => {
    const currentTime = Date.now();
    const elapsed = currentTime - global.lastDrawTime;
    if (elapsed >= global.FRAME_TIME) {
        global.lastDrawTime = currentTime - (elapsed % global.FRAME_TIME);
    }

    gameManager.draw();

    // Draw FPS counter
    if (global.DEBUG) {
        drawFPSCounter();
        drawRoomInfo();
    }
};

function drawFPSCounter() { // TODO: Without resetColors() this function fucks up the virtual palette. Need to fix!
    const fpsText = `fps: ${global.currentFPS}`;
    text(1, 1, fpsText, PICO8_COLORS[0]);
    text(3, 3, fpsText, PICO8_COLORS[0]);
    text(1, 3, fpsText, PICO8_COLORS[0]);
    text(3, 1, fpsText, PICO8_COLORS[0]);
    text(2, 2, fpsText, PICO8_COLORS[7]);
}

function drawRoomInfo() {
    const roomText = `room: ${state.room.x}, ${state.room.y}`;
    text(59, 1, roomText, PICO8_COLORS[0]);
    text(61, 3, roomText, PICO8_COLORS[0]);
    text(59, 3, roomText, PICO8_COLORS[0]);
    text(61, 1, roomText, PICO8_COLORS[0]);
    text(60, 2, roomText, PICO8_COLORS[7]);
}