import {Mouse} from "../framework/mouse";
import {APP_STATE, state} from "./state/state";
import {sc} from "../framework/scene";
import {w} from "../framework/widget";
import {B} from "../framework/b";
import {Button} from "../framework/components/button";
import {fileList, FileList} from "./components/file-list";

global.init = () => {
    setTransparentColor(255);

    const mainScene = sc(0, 0, [
        sc(4, 4, [
            // w(0, 0, () => text(0, 0, state.fileManager.currentDir)),
            new FileList(0, 0),
            // fileList(0, 0),
        ])
    ]);

    state.SPRITES = {
        CURSOR: 0
    }

    B.setScene(mainScene);
}

global.update = () => {
    Mouse.mouseUpdate(getMouseState());

    const currentTime = Date.now();
    const elapsed = currentTime - state.lastUpdateTime;

    if (elapsed >= state.FRAME_TIME) {
        if (state.appState === APP_STATE.DEFAULT) {
            B.getScene().update();
        }
    }
}

global.draw = () => {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        fillScreen(0);

        if (state.appState === APP_STATE.DEFAULT) {
            B.getScene().render();
        }

        drawSprite(Mouse.coords.x, Mouse.coords.y, state.SPRITES.CURSOR, false, false);
    }
}
