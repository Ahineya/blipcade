import {state} from "./state";
import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {destroy_object} from "./helpers";
import {Player} from "./player";
import {PICO8_COLORS} from "./colors";
import {psfx} from "./sound";

export class PlayerSpawn {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spr = 3;
        this.target = { x: this.x, y: this.y };
        this.y = 256;
        this.spd = { x: 0, y: -4 };
        this.state = 0;
        this.delay = 0;
        this.solids = false;
        this.flip = { x: false, y: false };
        this.create_hair();
    }

    create_hair() {
        this.hair = [];
        for (let i = 0; i < 5; i++) {
            this.hair.push({ x: this.x, y: this.y, size: Math.max(1, Math.min(2, 3 - i)) });
        }
    }

    update() {
        switch (this.state) {
            case 0: // jumping up
                if (this.y < this.target.y + 16) {
                    this.state = 1;
                    this.delay = 3;
                }
                break;
            case 1: // falling
                this.spd.y += 0.5;
                if (this.spd.y > 0 && this.delay > 0) {
                    this.spd.y = 0;
                    this.delay--;
                }
                if (this.spd.y > 0 && this.y > this.target.y) {
                    this.y = this.target.y;
                    this.spd = { x: 0, y: 0 };
                    this.state = 2;
                    this.delay = 5;
                    state.shake = 5;
                    init_object(Smoke, this.x, this.y + 4);
                    psfx(5);
                }
                break;
            case 2: // landing
                this.delay--;
                this.spr = 6;
                if (this.delay < 0) {
                    destroy_object(this);
                    init_object(Player, this.x, this.y);
                }
                break;
        }

        this.y += this.spd.y;
    }

    draw() {
        this.set_hair_color(state.max_djump);
        this.draw_hair();
        drawSprite(this.spr, this.x, this.y, this.flip.x, this.flip.y);
        this.unset_hair_color();
    }

    set_hair_color(djump) {
        // Implement color changing logic here
        // This might involve changing a global palette or shader
    }

    unset_hair_color() {
        // Reset color changing logic here
    }

    draw_hair() {
        let last = { x: this.x + 4 - (this.flip.x ? 2 : 0), y: this.y + 3 };
        for (let h of this.hair) {
            h.x += (last.x - h.x) / 1.5;
            h.y += (last.y + 0.5 - h.y) / 1.5;
            drawFilledCircle(h.x, h.y, h.size, PICO8_COLORS[8]); // Assuming 8 is the hair color
            last = h;
        }
    }
}