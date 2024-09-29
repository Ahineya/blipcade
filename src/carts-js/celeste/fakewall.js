import {Player} from "./player";
import {state} from "./state";
import {destroy_object, sign} from "./helpers";
import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {Fruit} from "./fruit";
import {psfx} from "./sound";

export class FakeWall {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.hitbox = {x: 0, y: 0, w: 16, h: 16};
        this.solids = true;
        this.collideable = true;
    }

    update() {
        let hit = this.check(Player, 0, 0);

        if (hit && hit.dash_effect_time > 0) {
            hit.spd.x = -sign(hit.spd.x) * 1.5;
            hit.spd.y = -1.5;
            hit.dash_time = -1;
            state.sfx_timer = 20;
            psfx(16);
            destroy_object(this);
            init_object(Smoke, this.x, this.y);
            init_object(Smoke, this.x + 8, this.y);
            init_object(Smoke, this.x, this.y + 8);
            init_object(Smoke, this.x + 8, this.y + 8);
            init_object(Fruit, this.x + 4, this.y + 4);
        }
    }

    draw() {
        drawSprite(64, this.x, this.y);
        drawSprite(65, this.x + 8, this.y);
        drawSprite(80, this.x, this.y + 8);
        drawSprite(81, this.x + 8, this.y + 8);

        // draw hitbox
        // drawRect(this.x + this.hitbox.x, this.y + this.hitbox.y, this.x + this.hitbox.w, this.y + this.hitbox.h, 8);
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            // Let's check for player collision, but thinking that player's hitbox is 1px bigger
            if (other instanceof type && other !== this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox - 1 &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy - 1 &&
                other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox + 1 &&
                other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy + 1
                ) {


                return other;
            }
        }
        return null;
    }
}