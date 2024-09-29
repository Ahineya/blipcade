import {hexToColor} from "./colors";

class B_ {
    #scene = null;
    #colors = {};

    setScene(scene) {
        this.#scene = scene;
    }

    getScene() {
        return this.#scene;
    }

    get SCREEN_SIZE() {
        return globalThis.SCREEN_SIZE;
    }

    addColor(name, hexColor) {
        this.#colors[name] = hexToColor(hexColor);
    }

    get colors() {
        return this.#colors;
    }
}

export const B = new B_();