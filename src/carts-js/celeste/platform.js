import {Player} from "./player";
import {state} from "./state";

export class Platform {
    constructor(x, y, dir) {
        this.x = x;
        this.y = y;
        this.dir = dir;
        this.hitbox = {x: 0, y: 0, w: 16, h: 4};
        this.spd = {x: 0.65 * this.dir, y: 0};
        this.solids = false;
        this.collideable = true;
    }

    update() {
        this.x += this.spd.x;
        if (this.x < -16) this.x = 128;
        if (this.x > 128) this.x = -16;

        let hit = this.check(Player, 0, 0);
        if (hit && hit.is_solid(0, 1)) {
            hit.move(this.spd.x, 0);
        }
    }

    draw() {
        drawSprite(11, this.x, this.y - 1, false, false);
        drawSprite(12, this.x + 8, this.y - 1, false, false);
    }

    check(type, ox, oy) { // TODO: move to prototype
        for (let other of state.objects) {
            if (other instanceof type && other != this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox - 1 &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy - 1  &&
                other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox + 1 &&
                other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy + 1
            ) {
                return other;
            }
        }
        return null;
    }
}