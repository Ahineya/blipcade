import {Widget} from "../widget";

export class Text extends Widget {
    constructor(x, y, text) {
        super(x, y);
        this.text = text;
    }

    render() {
        text(this.globalX, this.globalY, this.text);
    }
}