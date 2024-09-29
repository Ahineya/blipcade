import {destroy_object} from "./helpers";
import {PICO8_COLORS} from "./colors";

export class Smoke {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spr = 29;
        this.spd = {x: 0.3 + Math.random() * 0.2, y: -0.1};
        this.x += -1 + Math.random() * 2;
        this.y += -1 + Math.random() * 2;
        this.flip = {x: (Math.random() < 0.5), y: (Math.random() < 0.5)};
    }

    update() {
        this.spr += 0.2;
        if (this.spr >= 32) {
            destroy_object(this);
        }
    }

    draw() {
        drawSprite(Math.floor(this.spr), this.x, this.y, this.flip.x, this.flip.y);
    }
}