import {B} from "../framework/b";

export class Text {}

const GLYPH_WIDTH = 5; // TODO: make runtime provide this
const GLYPH_HEIGHT = 7; // TODO: make runtime provide this

Text.size = (text) => {
    return {
        width: GLYPH_WIDTH * text.length,
        height: GLYPH_HEIGHT
    }
}

Text.width = (text) => {
    return GLYPH_WIDTH * text.length;
}

Text.height = (text) => {
    return GLYPH_HEIGHT;
}

Text.center = (x, y, text) => {
    return {
        x: x - Text.width(text) / 2,
        y: y - Text.height(text) / 2
    }
}

Text.screenCenter = (text) => {
    return {
        x: B.SCREEN_SIZE.width / 2 - Text.width(text) / 2,
        y: B.SCREEN_SIZE.height / 2 - Text.height(text) / 2
    }
}
