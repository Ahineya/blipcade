import {state} from "./state";
import {Player} from "./player";
import {drawTime} from "./draw-time";
import {psfx} from "./sound";
import {PICO8_COLORS} from "./colors";

export class Flag {
    constructor(x, y) {
        this.x = x + 8;
        this.y = y;
        this.score = 0;
        this.show = false;
        for (let i = 1; i <= state.got_fruit.length; i++) {
            if (state.got_fruit[i]) {
                this.score++;
            }
        }
    }

    draw() {
        this.spr = 118 + Math.floor((state.frames / 5) % 3);
        drawSprite(this.spr, this.x, this.y);
        if (this.show) {
            drawFilledRectangle(32, 2, 96, 31, 0);
            drawSprite(26, 55, 6);
            resetColors();
            text(64, 9, "x" + this.score, PICO8_COLORS[7]);
            drawTime(49, 16);
            resetColors();
            text(48, 24, "DEATHS:" + state.deaths, PICO8_COLORS[7]);
            resetColors();

            // rectfill(32,2,96,31,0)
            // spr(26,55,6)
            // print("x"..this.score,64,9,7)
            // draw_time(49,16)
            // print("deaths:"..deaths,48,24,7)


        } else if (this.check(Player, 0, 0)) {
            psfx(55);
            state.sfx_timer = 30;
            this.show = true;
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
}