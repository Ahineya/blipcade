import {sc, Scene} from "../framework/scene";
import {Rect} from "../framework/components/rect";
import {B} from "../framework/b";
import {Button} from "../framework/components/button";
import {state} from "../state/state";
import {w} from "../framework/widget";

export const header = (x, y) => {
    return sc(new Scene(x, y), [ // Header scene
        new Rect(0, 0, B.SCREEN_SIZE.width, 12, 0, true),
        new Button(B.SCREEN_SIZE.width - 64, 0, 'Save', 0, 1, () => writeCart('test.blipcart', state.getUpdatedCart())),
        new Button(B.SCREEN_SIZE.width - 32, 0, 'Quit', 0, 1, () => quit()),
        w(0, 11, (x, y) => {
            drawLine(x, y, x + B.SCREEN_SIZE.width, y, 13);
        }),
    ])
}
