import {state} from "./state";
import {Player} from "./player";
import {destroy_object, level_index} from "./helpers";
import {init_object} from "./init_object";
import {Lifeup} from "./lifeup";
import {psfx} from "./sound";
import {approach} from "../graphics-lib/movement";

export class FlyFruit {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.start = y;
        this.fly = false;
        this.step = 0.5;
        this.sfx_delay = 8;
        this.hitbox = {x: 0, y: 0, w: 8, h: 8};
        this.spd = {x: 0, y: 0};
        this.rem = {x: 0, y: 0};
        this.solids = false;
        this.spr = 28;
    }

    update() {
        if (this.fly) {
            if (this.sfx_delay > 0) {
                this.sfx_delay--;
                if (this.sfx_delay <= 0) {
                    state.sfx_timer = 20;
                    psfx(14);
                }
            }
            this.spd.y = approach(this.spd.y, -3.5, 0.25);
            if (this.y < -16) {
                destroy_object(this);
            }
        } else {
            if (state.has_dashed) {
                this.fly = true;
            }
            this.step += 0.05;
            this.spd.y = Math.sin(this.step * 2 * Math.PI) * 0.5;
        }

        let hit = this.check(Player, 0, 0);
        if (hit) {
            hit.djump = state.max_djump;
            state.sfx_timer = 20;
            psfx(13);
            state.got_fruit[level_index()] = true;
            init_object(Lifeup, this.x, this.y);
            destroy_object(this);
        }
    }

    draw() {
        let off = 0;
        if (!this.fly) {
            let dir = Math.sin(this.step * 2 * Math.PI);
            if (dir < 0) {
                off = 1 + Math.max(0, Math.sign(this.y - this.start));
            }
        } else {
            off = (off + 0.25) % 3;
        }
        drawSprite(45 + off, this.x - 6, this.y - 2, true, false);
        drawSprite(this.spr, this.x, this.y);
        drawSprite(45 + off, this.x + 6, this.y - 2);
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            if (other instanceof type && other != this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy &&
                other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox &&
                other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy) {
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
