export function approach(val, target, amount) {
    return val > target ? Math.max(val - amount, target) : Math.min(val + amount, target);
}

export function approachExp(val, target, amount) {
    const diff = target - val;
    return val + diff * amount;
}

export const approach2D = (x, y, targetX, targetY, amount) => {
    const dx = targetX - x;
    const dy = targetY - y;
    const distanceSquared = dx * dx + dy * dy;

    if (distanceSquared <= amount * amount) {
        return { x: targetX, y: targetY };
    }

    const factor = amount * amount / distanceSquared;
    return {
        x: x + dx * factor,
        y: y + dy * factor
    };
};