export function rgbToColor(r, g, b) {
    const rIndex = Math.min(Math.floor(r / 51), 5);
    const gIndex = Math.min(Math.floor(g / 36), 7);
    const bIndex = Math.min(Math.floor(b / 63), 4);
    return (40 * rIndex) + (5 * gIndex) + bIndex;
}

export function hexToColor(hex) {
    const r = parseInt(hex.slice(1, 3), 16);
    const g = parseInt(hex.slice(3, 5), 16);
    const b = parseInt(hex.slice(5, 7), 16);
    return rgbToColor(r, g, b);
}