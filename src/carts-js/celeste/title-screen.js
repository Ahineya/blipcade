import {state} from "./state";
import {load_room} from "./load_room";

export function titleScreen() {
    state.got_fruit = new Array(30).fill(false);
    state.frames = 0;
    state.deaths = 0;
    state.max_djump = 1;
    state.start_game = false;
    state.start_game_flash = 0;
    // playMusic(40, 0, 7);  // You'll need to implement this function

    load_room(7, 3);
}

function beginGame() {
    state.frames = 0;
    state.seconds = 0;
    state.minutes = 0;
    state.music_timer = 0;
    state.start_game = false;
    // playMusic(0, 0, 7);  // You'll need to implement this function
    load_room(0, 0);
}