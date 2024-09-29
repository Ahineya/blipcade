import {Player} from "./player";
import {state} from "./state";
import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {psfx} from "./sound";

export class Balloon {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.hitbox = {x: -1, y: -1, w: 10, h: 10};
        this.spr = 22;
        this.offset = Math.random();
        this.start = y;
        this.timer = 0;
    }

    update() {
        if (this.spr == 22) {
            this.offset += 0.01;
            this.y = this.start + Math.sin(this.offset) * 2;
            let hit = this.check(Player, 0, 0);
            if (hit && hit.djump < state.max_djump) {
                psfx(6);
                init_object(Smoke, this.x, this.y);
                hit.djump = state.max_djump;
                this.spr = 0;
                this.timer = 60;
            }
        } else if (this.timer > 0) {
            this.timer--;
        } else {
            psfx(7);
            init_object(Smoke, this.x, this.y);
            this.spr = 22;
        }
    }

    draw() {
        if (this.spr == 22) {
            drawSprite(13 + Math.floor((this.offset * 8) % 3), this.x, this.y + 6);
            drawSprite(this.spr, this.x, this.y);
        }
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