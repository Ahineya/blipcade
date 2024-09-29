import {init_object} from "./init_object";
import {Smoke} from "./smoke";
import {Spring} from "./spring";
import {psfx} from "./sound";

export function break_fall_floor(obj) {
    if (obj.state == 0) {
        psfx(15);
        obj.state = 1;
        obj.delay = 15;
        init_object(Smoke, obj.x, obj.y);
        let hit = obj.check(Spring, 0, -1);
        if (hit) {
            break_spring(hit);
        }
    }
}

export function break_spring(obj) {
    obj.hide_in = 15;
}