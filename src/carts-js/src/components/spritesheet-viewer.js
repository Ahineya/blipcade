import {Widget} from "../framework/widget";
import {state} from "../state/state";
import {B} from "../framework/b";

export class SpritesheetViewer extends Widget {
    constructor(x, y, spritesheet, sprites) {
        super(x, y);
        // state.setSpritesheet(spritesheet);
        // state.sprites = [];
        // for (let i = 0; i < sprites.length; i += 5) {
        //     state.sprites.push({
        //         x: sprites[i],
        //         y: sprites[i + 1],
        //         width: sprites[i + 2],
        //         height: sprites[i + 3],
        //         flags: sprites[i + 4]
        //     });
        // }
    }

    render() {
        const sheetSize = Math.ceil(Math.sqrt(state.spritesheet.length));
        drawRectData(this.globalX, this.globalY, this.globalX + sheetSize, this.globalY + sheetSize, true, state.spritesheet, 1);

        // Draw a rectangle around the active sprite with color 4
        const sprite = state.sprites[state.activeSprite];
        if (!sprite) {
            return;
        }
        drawRect(this.globalX + sprite.x - 1, this.globalY + sprite.y - 1, this.globalX + sprite.x + sprite.width, this.globalY + sprite.y + sprite.height, B.colors.WHITE);
        setTransparentColor(15);
        drawRect(this.globalX + sprite.x - 2, this.globalY + sprite.y - 2, this.globalX + sprite.x + sprite.width + 1, this.globalY + sprite.y + sprite.height + 1, 0);
        setTransparentColor(255);
    }
}