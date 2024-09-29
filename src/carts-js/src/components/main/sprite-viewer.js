import {sc} from "../../framework/scene";
import {SpriteViewer} from "../sprite-viewer";

export const spriteViewer = (x, y) => {
    return sc(x, y, [ // Sprite viewer scene
        new SpriteViewer(0, 0)
    ])
}