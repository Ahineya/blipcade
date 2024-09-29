import { state } from "./state";
import { Player } from "./player";
import { psfx } from "./sound";
import {PICO8_COLORS} from "./colors";

export class Message {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.tile = 86;
        this.last = 0;
        this.index = 0;
        this.text = "-- celeste mountain --#this memorial to those# perished on the climb";
        this.off = { x: 8, y: 96 };
    }

    update() {
        let hit = this.check(Player, 4, 0);

        if (hit) {
            if (this.index < this.text.length) {
                this.index += 0.5;
                if (this.index >= this.last + 1) {
                    this.last += 1;
                    psfx(35);
                }
            }
        } else {
            this.index = 0;
            this.last = 0;
        }
    }

    draw() {
        if (this.check(Player, 4, 0)) {
            this.off = { x: 8, y: 96 };
            for (let i = 0; i < Math.floor(this.index); i++) {
                if (this.text[i] !== "#") {
                    // Assuming you have functions similar to PICO-8's rectfill and print
                    drawFilledRectangle(this.off.x - 2, this.off.y - 2, this.off.x + 7, this.off.y + 6, PICO8_COLORS[7]);
                    text(this.off.x, this.off.y, this.text[i], PICO8_COLORS[1]);
                    resetColors();
                    this.off.x += 5;
                } else {
                    this.off.x = 8;
                    this.off.y += 7;
                }
            }

            // text(this.off.x, this.off.y, ">", PICO8_COLORS[7]);
        }
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            if (other instanceof type && other !== this &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + ox &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + oy &&
                other.x + other.hitbox.x < this.x + ox + 16 &&
                other.y + other.hitbox.y < this.y + oy + 16) {
                return other;
            }
        }
        return null;
    }
}