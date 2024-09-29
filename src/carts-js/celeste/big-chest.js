import {Player} from "./player";
import {state} from "./state";
import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {Orb} from "./orb";
import {pmusic, psfx} from "./sound";
import {PICO8_COLORS} from "./colors";

export class BigChest {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.state = 0;
        this.hitbox = {x: 0, y: 0, w: 16, h: 16};
        this.lid_offset = 0;
        this.particles = [];
    }

    update() {
        if (this.state == 0) {
            let hit = this.check(Player, 0, 8);
            if (hit && hit.is_solid(0, 1)) {
                pmusic(-1, 500, 7);
                psfx(37);
                state.pause_player = true;
                hit.spd.x = 0;
                hit.spd.y = 0;
                this.state = 1;
                init_object(Smoke, this.x, this.y);
                init_object(Smoke, this.x + 8, this.y);
                this.timer = 60;
            }
        } else if (this.state == 1) {
            state.shake = 5;
            state.flash_bg = true;

            this.timer--;
            this.lid_offset = Math.max(this.lid_offset - 1, 0);
            if (this.timer <= 45 && this.particles.length < 50) {
                this.particles.push({
                    x: 1 + Math.random() * 14,
                    y: 0,
                    h: 32 + Math.random() * 32,
                    spd: 8 + Math.random() * 8
                });
            }
            if (this.timer < 0) {
                this.state = 2;
                this.particles = [];
                state.flash_bg = false;
                state.new_bg = true;
                init_object(Orb, this.x + 4, this.y + 4);
                state.pause_player = false;
            }
            state.shake = 5;
        }
    }

    draw() {
        if (this.state == 0) {
            drawSprite(96, this.x, this.y);
            drawSprite(97, this.x + 8, this.y);
        } else {
            drawSprite(98, this.x, this.y - this.lid_offset);
            drawSprite(99, this.x + 8, this.y - this.lid_offset);
        }
        drawSprite(112, this.x, this.y + 8);
        drawSprite(113, this.x + 8, this.y + 8);

        if (this.state == 1) {
            for (let p of this.particles) {
                p.y += p.spd;
                let height = Math.min(p.h, this.y + 8 - p.y);
                if (height > 0) {
                    drawLine(this.x + p.x, this.y + 8 - p.y, this.x + p.x, this.y + 8 - p.y + height, PICO8_COLORS[7]);
                }
            }
        }
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            if (other instanceof type && other != this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox + 1 &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy + 1 &&
                other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox + 1 &&
                other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy + 1) {
                return other;
            }
        }
        return null;
    }
}