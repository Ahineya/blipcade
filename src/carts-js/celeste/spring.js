import {Player} from "./player";
import {FallFloor} from "./fallfloor";
import {init_object} from "./init_object";
import {state} from "./state";
import {Smoke} from "./smoke";
import {break_fall_floor} from "./break_fall_floor";
import {psfx} from "./sound";

export class Spring {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.hitbox = {x: 0, y: 0, w: 8, h: 8};
        this.spr = 18;
        this.hide_in = 0;
        this.hide_for = 0;
    }

    update() {
        if (this.hide_for > 0) {
            this.hide_for--;
            if (this.hide_for <= 0) {
                this.spr = 18;
                this.delay = 0;
            }
        } else if (this.spr == 18) {
            let hit = this.check(Player, 0, 0);
            if (hit && hit.spd.y >= 0) {
                this.spr = 19;
                hit.y = this.y - 4;
                hit.spd.x *= 0.2;
                hit.spd.y = -3;
                hit.djump = state.max_djump;
                this.delay = 10;
                init_object(Smoke, this.x, this.y);

                let below = this.check(FallFloor, 0, 1);
                if (below) {
                    break_fall_floor(below);
                }

                psfx(8);
            }
        } else if (this.delay > 0) {
            this.delay--;
            if (this.delay <= 0) {
                this.spr = 18;
            }
        }
        if (this.hide_in > 0) {
            this.hide_in--;
            if (this.hide_in <= 0) {
                this.hide_for = 60;
                this.spr = 0;
            }
        }
    }

    draw() {
        drawSprite(this.spr, this.x, this.y);
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
}
