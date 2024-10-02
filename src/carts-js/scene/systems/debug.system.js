import {getScale} from "./movement.system";
import {state} from "../state/state";
import {PICO8_COLORS} from "../../celeste/colors";

class DebugSystem {
  constructor() {
    this.name = 'DebugSystem';
  }

  draw() {
    drawColliders();
    drawFPSCounter();
  }
}

export const debugSystem = new DebugSystem();

function drawFPSCounter() {
    const fpsText = `fps: ${state.currentFPS}`;
    text(fpsText,1, 2,  PICO8_COLORS[0]);
    text(fpsText, 3, 2,  PICO8_COLORS[0]);
    text(fpsText,2, 3,  PICO8_COLORS[0]);
    text(fpsText,2, 1,  PICO8_COLORS[0]);
    text(fpsText,2, 2,  PICO8_COLORS[7]);
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

            const originX = x + width * ox * scale;
            const originY = y + height * oy * scale;

            // Graphics.drawLine(startX + originX, startY + originY, endX + originX, endY + originY, 0xfe);
        }
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
}