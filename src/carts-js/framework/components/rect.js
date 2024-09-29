import {Widget} from "../widget";

export class Rect extends Widget {
    constructor(x, y, width, height, color, isFilled = false) {
        super(x, y);
        this.width = width;
        this.height = height;
        this.color = color;
        this.isFilled = isFilled;
    }

    render() {
        this.isFilled
            ? drawFilledRectangle(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.color)
            : drawRect(this.globalX, this.globalY, this.globalX + this.width, this.globalY + this.height, this.color);
    }
}
