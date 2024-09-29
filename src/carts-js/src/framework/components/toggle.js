import {Widget} from "../widget";
import {B} from "../b";
import {Mouse} from "../mouse";

export class Toggle extends Widget {
    constructor(x, y, color, hoverColor, onClick, isToggledFn = () => false) {
        super(x, y);
        this.onClick = onClick;
        this.color = color;
        this.originalColor = color;
        this.hoverColor = hoverColor;
        this.isHovered = false;
        this.isToggledFn = isToggledFn;

        this.width = 7; // 7 is the width of a character
        this.height = 7; // 8 is the height of a character, 2 is padding

        Mouse.addEventListener('leftDown', (pos) => this.clickListener(pos));
        Mouse.addEventListener('move', (pos) => this.hoverListener(pos));
    }

    isPointInside(x, y) {
        return x >= this.globalX && x < this.globalX + this.width &&
            y >= this.globalY && y < this.globalY + this.height;
    }

    hoverListener({x, y}) {
        this.isHovered = this.isPointInside(x, y);
    }

    clickListener({x, y}) {
        if (this.isPointInside(x, y)) {
            this.isToggled = !this.isToggledFn();
            this.onClick(this.isToggled);
        }
    }

    render() {
        if (this.isHovered) {
            drawFilledCircle(this.globalX + 3, this.globalY + 3, 3, this.hoverColor);
        }

        drawCircle(this.globalX + 3, this.globalY + 3, 3, this.originalColor);

        if (this.isToggledFn()) {
            drawFilledCircle(this.globalX + 3, this.globalY + 3, 2, B.colors.YELLOW);
        }
    }
}