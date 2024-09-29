import {state} from "./state";
import {PICO8_COLORS} from "./colors";

export function drawTime(x, y) {
    let s = state.seconds;
    let m = state.minutes % 60;
    let h = Math.floor(state.minutes / 60);

    let timeStr = (h < 10 ? "0" + h : h) + ":" +
        (m < 10 ? "0" + m : m) + ":" +
        (s < 10 ? "0" + s : s);

    drawFilledRectangle(x, y, x + 32, y + 6, PICO8_COLORS[3]);
    // drawText(timeStr, x + 1, y + 1, 7);
    resetColors();
    text(x + 1, y + 1, timeStr, PICO8_COLORS[7]);
    resetColors();
}