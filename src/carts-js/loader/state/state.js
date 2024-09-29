import {ParticleManager} from "../particle-manager";

export const APP_STATE = {
    LOADING: 'loading',
    LOADED: 'loaded',
    OS: 'os',
}

class State {

    appState = APP_STATE.LOADING;

    FONT_SIZE = {
        width: 5,
        height: 7
    }

    FPS = 30;
    FRAME_TIME = 1000 / this.FPS;
    lastUpdateTime = Date.now();
    lastDrawTime = Date.now();

    frames = 0;
    seconds = 0;

    objects = [];

    particleManager = null;

    constructor() {
    }
}

export const state = new State();