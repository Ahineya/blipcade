import {destroy_object} from "./helpers";
import {PICO8_COLORS} from "./colors";

export class Lifeup {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spd = {x: 0, y: -0.25};
        this.rem = {x: 0, y: 0};
        this.duration = 30;
        this.flash = 0;
        this.solids = false;
    }

    update() {
        this.duration--;
        if (this.duration <= 0) {
            destroy_object(this);
        }
    }

    draw() {
        this.flash += 0.5;
        text(this.x - 2, this.y, "1000", PICO8_COLORS[7 + this.flash % 2]);
    }

    move(ox, oy) {
        let amount;
        // [x] get move amount
        this.rem.x += ox;
        amount = Math.floor(this.rem.x + 0.5);
        this.rem.x -= amount;
        this.move_x(amount, 0);

        // [y] get move amount
        this.rem.y += oy;
        amount = Math.floor(this.rem.y + 0.5);
        this.rem.y -= amount;
        this.move_y(amount);
    }

    move_x(amount, start) {
        if (this.solids) {
            let step = Math.sign(amount);
            for (let i = start; i !== Math.abs(amount); i++) {
                this.x += step;
            }
        } else {
            this.x += amount;
        }
    }

    move_y(amount) {
        if (this.solids) {
            let step = Math.sign(amount);
            for (let i = 0; i !== Math.abs(amount); i++) {
                this.y += step;
            }
        } else {
            this.y += amount;
        }
    }
}
