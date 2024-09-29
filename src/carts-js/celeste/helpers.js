import {state} from "./state";

export function tile_at(x, y) {
    return getSpriteIndexAtMapCoords(state.room.x * 16 + x, state.room.y * 16 + y, 0);
}

export function solid_at(x, y, w, h) {
    return tile_flag_at(x, y, w, h, 0);
}

export function ice_at(x, y, w, h) {
    return tile_flag_at(x, y, w, h, 4);
}

export function tile_flag_at(x, y, w, h, flag) {
    for (let i = Math.max(0, Math.floor(x / 8)); i <= Math.min(15, (x + w - 1) / 8); i++) {
        for (let j = Math.max(0, Math.floor(y / 8)); j <= Math.min(15, (y + h - 1) / 8); j++) {
            if (fget(tile_at(i, j), flag)) {
                return true;
            }
        }
    }
    return false;
}

export function fget(tile, flag) {
    let isFlag = isSpriteFlag(tile, flag);
    return isFlag;
}

export function is_title() {
    return state.room.x === 7 && state.room.y === 3;
}

export function level_index() {
    return state.room.x % 8 + state.room.y * 8;
}

// export function next_room() {
//     if (state.room.x == 7) {
//         load_room(0, state.room.y + 1);
//     } else {
//         load_room(state.room.x + 1, state.room.y);
//     }
// }


export function restart_room() {
    state.will_restart = true;
    state.delay_restart = 15;
}



export function destroy_object(obj) {
    let index = state.objects.indexOf(obj);
    if (index > -1) {
        state.objects.splice(index, 1);
    }
}

export function sign(number) {
    return number > 0 ? 1 : number < 0 ? -1 : 0;
}