import {destroy_object} from "./helpers";
import {Player} from "./player";
import {state} from "./state";
import {psfx} from "./sound";
import {PICO8_COLORS} from "./colors";
import {approach} from "../graphics-lib/movement";

export class Orb {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spd = {x: 0, y: -4};
        this.rem = {x: 0, y: 0};
        this.solids = false;
        this.particles = [];
    }

    update() {
        this.spd.y = approach(this.spd.y, 0, 0.5);
        let hit = this.check(Player, 0, 0);
        if (this.spd.y == 0 && hit) {
            state.music_timer = 45;
            psfx(51);
            state.freeze = 10;
            state.shake = 10;
            destroy_object(this);
            state.max_djump = 2;
            hit.djump = 2;
        }
    }

    draw() {
        drawSprite(102, this.x, this.y);
        // log("Orb draw not fully implemented");
        let off = state.frames / 30;
        for (let i = 0; i < 8; i++) {
            const angle = (off + i / 8) * 2 * Math.PI;
            drawFilledCircle(
                this.x + 4 + Math.cos(angle) * 8,
                this.y + 4 + Math.sin(angle) * 8,
                1,
                PICO8_COLORS[7]
            );
        }
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            if (other instanceof type && other != this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + ox &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + oy &&
                other.x + other.hitbox.x < this.x + 8 + ox &&
                other.y + other.hitbox.y < this.y + 8 + oy) {
                return other;
            }
        }
        return null;
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