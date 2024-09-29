export function lerp(value, x1, x2, y1, y2) {
    return y1 + (value - x1) * (y2 - y1) / (x2 - x1);
}