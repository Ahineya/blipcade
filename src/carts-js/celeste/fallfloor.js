import {Player} from "./player";
import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {state} from "./state";
import {break_fall_floor} from "./break_fall_floor";
import {psfx} from "./sound";

export class FallFloor {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.hitbox = {x: 0, y: 0, w: 8, h: 8};
        this.state = 0;
        this.solid = true;
        this.collideable = true;
    }

    update() {
        if (this.state == 0) {
            if (this.check(Player, 0, -1) || this.check(Player, -1, 0) || this.check(Player, 1, 0)) {
                break_fall_floor(this);
            }
        } else if (this.state == 1) {
            this.delay--;
            if (this.delay <= 0) {
                this.state = 2;
                this.delay = 60;
                this.collideable = false;
            }
        } else if (this.state == 2) {
            this.delay--;
            if (this.delay <= 0 && !this.check(Player, 0, 0)) {
                psfx(7);
                this.state = 0;
                this.collideable = true;
                init_object(Smoke, this.x, this.y);
            }
        }
    }

    draw() {
        if (this.state != 2) {
            if (this.state != 1) {
                drawSprite(23, this.x, this.y);
            } else {
                drawSprite(23 + Math.floor((15 - this.delay) / 5), this.x, this.y);
            }
        }
        // log("FALLFLOOR: draw not implemented");
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