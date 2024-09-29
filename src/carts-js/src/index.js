import {B} from "./framework/b";
import {Mouse} from "./framework/mouse";
import {sc} from "./framework/scene";
import {state} from "./state/state";
import {header} from "./components/header";
import {main} from "./components/main/main";

global.init = () => {
    B.addColor('BLACK', '#000000');
    B.addColor('DARK_BLUE', '#06064a');
    B.addColor('WHITE', '#ffffff');
    B.addColor('YELLOW', '#f0f000');

    state.cart = readCart('../carts/celeste.blipcart');
    state.loadCart();

    const mainScene = sc(0, 0, [
        header(0, 0),
        main(4, 12 + 4)
    ]);

    state.SPRITES = {
        CURSOR: 0
    }

    B.setScene(mainScene);
}

global.update = () => {
    Mouse.mouseUpdate(getMouseState());
}

global.draw = () => {
    fillScreen(B.colors.DARK_BLUE);


    B.getScene().render();


    drawSprite(Mouse.coords.x, Mouse.coords.y, state.SPRITES.CURSOR);
}
