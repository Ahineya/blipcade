import {state} from "./state";
import {init_object} from "./init_object";
import {destroy_object} from "./helpers";
import {Fruit} from "./fruit";
import {psfx} from "./sound";

export class Chest {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.start = this.x;
        this.timer = 20;
        this.hitbox = {x: 0, y: 0, w: 16, h: 16};
    }

    update() {
        if (state.has_key) {
            this.timer--;
            this.x = this.start - 1 + Math.random() * 3;
            if (this.timer <= 0) {
                state.sfx_timer = 20;
                psfx(16);
                init_object(Fruit, this.x, this.y - 4);
                destroy_object(this);
            }
        }
    }

    draw() {
        drawSprite(20, this.x, this.y);
    }
}
