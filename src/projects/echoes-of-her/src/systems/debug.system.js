import {getScale} from "./movement.system.js";
import {state} from "../state/state.js";

class DebugSystem {
    constructor() {
        this.name = 'DebugSystem';
    }

    draw() {
        // drawColliders();
        // drawFPSCounter();
    }
}

export const debugSystem = new DebugSystem();

function drawFPSCounter() {
    const fpsText = `fps: ${state.currentFPS}`;
    text(fpsText, 1, 2, 0);
    text(fpsText, 3, 2, 0);
    text(fpsText, 2, 3, 0);
    text(fpsText, 2, 1, 0);
    text(fpsText, 2, 2, 0xfe);
}

function drawColliders() {
    ECS.forEachEntity(["Player", "Sprite", "Collider"], (_, player, sprite, collider) => {
        const {vertices} = collider;

        for (let i = 0; i < vertices.length; i++) {

            const start = vertices[i];
            const end = vertices[(i + 1) % vertices.length];

            // Let's calculate where the line should be drawn in the world based on the player sprite position and origin
            const {position: {x, y}, origin: {x: ox, y: oy}, size: {width, height}} = sprite; // Position is always top-left corner of the sprite. Origin is from 0 to 1, where 0.5 is the center

            // We need to calculate the actual position of the origin in the world. And let's not forget about scale
            const scale = getScale(y);

            const startX = start.x * scale;
            const startY = start.y * scale;
            const endX = end.x * scale;
            const endY = end.y * scale;

            const originX = Math.round(x + width * ox * scale);
            const originY = Math.round(y + height * oy * scale);

            // Graphics.drawLine(startX + originX, startY + originY, endX + originX, endY + originY, 0xfe);
        }
    });

    ECS.forEachEntity(["Sprite"], (entity, sprite) => {
        const originX = Math.round(sprite.position.x);

        let originY = Math.round(sprite.position.y);
        const player = ECS.getComponent(entity, "Player");
        if (player) {
            const playerScale = ECS.getComponent(entity, "PlayerScale");
            originY = Math.round(sprite.position.y);
        }

        // Graphics.drawFilledCircle(originX, originY, 2, 0xfe);
    });

    ECS.forEachEntity(["Collider", "Visible"], (entity, collider, visible) => {
        if (ECS.getComponent(entity, "Player")) {
            return;
        }

        if (!visible) {
            return;
        }

        const {vertices} = collider;

        for (let i = 0; i < vertices.length; i++) {
            const start = vertices[i];
            const end = vertices[(i + 1) % vertices.length];

            Graphics.drawLine(start.x, start.y, end.x, end.y, 0xfe);
        }
    });

    ECS.forEachEntity(["Player"], (_, player) => {
        const navMesh = Pathfinding.getNavMesh(player.navMeshIndex); // array of polygons, [{vertices: [{x, y}, {x, y}, ...]}, ...]

        for (let i = 0; i < navMesh.length; i++) {
            const polygon = navMesh[i];
            const vertices = polygon.vertices;

            for (let j = 0; j < vertices.length; j++) {
                const start = vertices[j];
                const end = vertices[(j + 1) % vertices.length];

                // Graphics.drawLine(start.x, start.y, end.x, end.y, 35);
            }
        }

        const path = player.path;

        if (path.length > 0) {
            for (let i = 0; i < path.length - 1; i++) {
                const p1 = path[i];
                const p2 = path[i + 1];
                // Graphics.drawLine(p1.x, p1.y, p2.x, p2.y, 0xfe);
            }
        }
    });
}