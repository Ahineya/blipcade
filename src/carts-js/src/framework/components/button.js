import {Widget} from "../widget";
import {Mouse} from "../mouse";

export class Button extends Widget {
    constructor(x, y, text, color, hoverColor, onClick) {
        super(x, y);
        this.text = text;
        this.onClick = onClick;
        this.color = color;
        this.originalColor = color;
        this.hoverColor = hoverColor;
        this.isHovered = false;

        this.width = text.length * 7 + 4; // 7 is the width of a character, 6 is padding
        this.height = 8 + 2; // 8 is the height of a character, 2 is padding

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
            this.onClick();
        }
    }

    render() {
        if (this.isHovered) {
            drawFilledRectangle(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.hoverColor);
        }

        drawRect(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.originalColor);
        text(this.globalX + 3, this.globalY + 1, this.text);
    }
}