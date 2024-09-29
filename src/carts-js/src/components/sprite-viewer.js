import {Widget} from "../framework/widget";
import {state} from "../state/state";
import {B} from "../framework/b";

export class SpriteViewer extends Widget {
    constructor(x, y) {
        super(x, y);
        this.lastSpritesheet = null;
    }

    render() {
        const sprite = state.sprites[state.activeSprite];

        if (!sprite) {
            return;
        }

        const zoom = state.zoom;
        const sheetSize = Math.ceil(Math.sqrt(state.spritesheet.length));


        // Check if we need to update our spritesheet reference
        if (this.lastSpritesheet !== state.spritesheet) {
            this.lastSpritesheet = state.spritesheet;
        }

        setClippingRect(this.globalX, this.globalY, B.SCREEN_SIZE.width / 2 - 8, B.SCREEN_SIZE.height / 2 - 16);

        // Draw the sprite using the new drawRectDataSlice function
        drawRectDataSlice(
            this.globalX,
            this.globalY,
            sprite.width,
            sprite.height,
            sheetSize,
            sprite.x,
            sprite.y,
            true,
            this.lastSpritesheet,
            zoom
        );

        setClippingRect(0, 0, B.SCREEN_SIZE.width, B.SCREEN_SIZE.height);
    }
}