import {Widget} from "../widget";
import {Mouse} from "../mouse";

export class Button extends Widget {
    constructor({x, y, text, color, hoverColor, onClick, textColor = 0xfe, border = true}) {
        super(x, y);
        this.text = text;
        this.onClick = onClick;
        this.color = color;
        this.originalColor = color;
        this.hoverColor = hoverColor;
        this.isHovered = false;
        this.textColor = textColor;
        this.border = border;

        this.width = text.length * global.GLYPH_SIZE.width + 4; // 7 is the width of a character, 6 is padding
        this.height = global.GLYPH_SIZE.height + 2; // 8 is the height of a character, 2 is padding

        this._clickListener = this.clickListener.bind(this);
        this._hoverListener = this.hoverListener.bind(this);

        Mouse.addEventListener('leftDown', this._clickListener);
        Mouse.addEventListener('move', this._hoverListener);
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

    destroy() {
        Mouse.removeEventListener('leftDown', this._clickListener);
        Mouse.removeEventListener('move', this._hoverListener);
    }

    render() {
        if (this.isHovered) {
            drawFilledRectangle(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.hoverColor);
        }

        if (this.border) {
            drawRect(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.originalColor);
        }

        text(this.globalX + 3, this.globalY + 1, this.text, this.textColor);
    }
}