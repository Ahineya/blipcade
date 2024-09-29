import {state} from "./state";
import {Player} from "./player";
import {destroy_object} from "./helpers";
import {psfx} from "./sound";

export class Key {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spr = 8;
        this.hitbox = {x: 0, y: 0, w: 8, h: 8};
        this.collideable = true;
        this.flip = {x: false, y: false};
    }

    update() {
        let was = Math.floor(this.spr);
        this.spr = 8 + (Math.sin(state.frames / 30) + 0.5) * 1;
        let is = Math.floor(this.spr);
        if (is === 9 && is !== was) {
            this.flip = {x: !this.flip.x, y: false};
        }
        if (this.check(Player, 0, 0)) {
            psfx(23);
            state.sfx_timer = 10;
            destroy_object(this);
            state.has_key = true;
        }
    }

    draw() {
        drawSprite(this.spr, this.x, this.y, this.flip.x, this.flip.y);
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