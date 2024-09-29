import {destroy_object, level_index} from "./helpers";
import {drawTime} from "./draw-time";
import {state} from "./state";

export class RoomTitle {
    constructor() {
        this.delay = 5;
    }

    update() {
        this.delay--;
        if (this.delay < -30) {
            destroy_object(this);
        }
    }

    draw() {
        if (this.delay < 0) {
            let level = (1 + level_index()) * 100;
            let tt = level + " M";
            if (level_index() === 30) {
                tt = "SUMMIT";
            } else if (state.room.x == 3 && state.room.y == 1) {
                tt = "OLD SITE";
            }
            drawFilledRectangle(24, 58, 104, 70, 0);
            text(64 - tt.length * 2, 62, tt, 0xfe);
            drawTime(4, 4);
        }
    }
}
