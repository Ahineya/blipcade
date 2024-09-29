import {FileManager} from "../model/file-manager";

export const APP_STATE = {
    DEFAULT: 'default',
}

class State {
    appState = APP_STATE.DEFAULT;

    CARTS_DIR = fs.getCartsDir();

    FPS = 30;
    FRAME_TIME = 1000 / this.FPS;
    lastUpdateTime = Date.now();
    lastDrawTime = Date.now();

    frames = 0;
    seconds = 0;

    fileManager = new FileManager();

    constructor() {
    }
}

export const state = new State();