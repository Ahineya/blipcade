import {state} from "./state";
import {load_room} from "./load_room";
import {level_index} from "./helpers";
import {pmusic} from "./sound";

export function next_room() {
    let next_room_x, next_room_y;

    // Determine next room coordinates
    if (state.room.x === 7) {
        next_room_x = 0;
        next_room_y = state.room.y + 1;
    } else {
        next_room_x = state.room.x + 1;
        next_room_y = state.room.y;
    }

    // Play music based on current room before moving
    if (state.room.x === 2 && state.room.y === 1) {
        pmusic(30, 500, 7);
    } else if (state.room.x === 3 && state.room.y === 1) {
        pmusic(20, 500, 7);
    } else if (state.room.x === 4 && state.room.y === 2) {
        pmusic(30, 500, 7);
    } else if (state.room.x === 5 && state.room.y === 3) {
        pmusic(30, 500, 7);
    }

    // Load the next room
    load_room(next_room_x, next_room_y);
}