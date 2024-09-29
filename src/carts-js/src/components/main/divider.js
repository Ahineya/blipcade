import {w} from "../../framework/widget";
import {B} from "../../framework/b";

export const divider = (x, y) => {
    return w(x, y, (x, y) => { // Divider
        drawLine(x, y, x, y + B.SCREEN_SIZE.height / 2 - 7, 13);
    });
}