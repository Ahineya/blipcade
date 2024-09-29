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

export const PICO8_COLORS = {
    0: 0x00,
    1: 0x2E,
    2: 0x56,
    3: 0x15,
    4: 0x83,
    5: 0x5B,
    6: 0xBC,
    7: 0xEF,
    8: 0xC9,
    9: 0xE1,
    10: 0xEC,
    11: 0x1F,
    12: 0x45,
    13: 0x89,
    14: 0xDA,
    15: 0xE9
};
