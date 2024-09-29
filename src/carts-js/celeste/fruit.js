import {Player} from "./player";
import {state} from "./state";
import {destroy_object, level_index} from "./helpers";
import {init_object} from "./init_object";
import {Lifeup} from "./lifeup";
import {psfx} from "./sound";

export class Fruit {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.start = y;
        this.off = 0;
        this.hitbox = {x: 0, y: 0, w: 8, h: 8};
        this.spr = 26;
    }

    update() {
        let hit = this.check(Player, 0, 0);
        if (hit) {
            hit.djump = state.max_djump;
            state.sfx_timer = 20;
            psfx(13);
            state.got_fruit[level_index()] = true;
            init_object(Lifeup, this.x, this.y);
            destroy_object(this);
        }
        this.off += 1;
        this.y = this.start + Math.sin(this.off / 40) * 2.5;
    }

    draw() {
        drawSprite(this.spr, this.x, this.y);
        // log("Fruit draw not implemented");
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