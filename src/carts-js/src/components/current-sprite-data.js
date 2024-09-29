import {Widget} from "../framework/widget";
import {state} from "../state/state";

export class CurrentSpriteData extends Widget {
    constructor(x, y, data = "index") {
        super(x, y);
        this.data = data;
    }

    render() {
        switch (this.data) {
            case "index":
                text(this.globalX, this.globalY, state.activeSprite);
                break;
            case "index/total":
                text(this.globalX, this.globalY, `${state.activeSprite}/${state.sprites.length}`);
                break;
            case "x": {
                const sprite = state.sprites[state.activeSprite];
                if (!sprite) {
                    return;
                }
                text(this.globalX, this.globalY, `${sprite.x}px`);
            }
                break;
            case "y": {
                const sprite = state.sprites[state.activeSprite];
                if (!sprite) {
                    return;
                }
                text(this.globalX, this.globalY, `${sprite.y}px`);
            }
                break;
            case "width": {
                const sprite = state.sprites[state.activeSprite];
                if (!sprite) {
                    return;
                }
                text(this.globalX, this.globalY, `${sprite.width}px`);
            }
                break;
            case "height": {
                const sprite = state.sprites[state.activeSprite];
                if (!sprite) {
                    return;
                }
                text(this.globalX, this.globalY, `${sprite.height}px`);
            }
                break;
            default:
                text(this.globalX, this.globalY, "Unknown data");
        }
    }
}