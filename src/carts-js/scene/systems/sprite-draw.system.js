import {getScale} from "./movement.system";

class SpriteDrawSystem {
    constructor() {
        this.playerEntity = null;
        this.bgOverlayEntity = null;
    }

    init() {
        ECS.forEachEntity(["Player"], (entity) => {
            this.playerEntity = entity;
        });

        ECS.forEachEntity(["Tag"], (entity, tag) => {
            if (tag !== "BackgroundOverlay") {
                return;
            }

            this.bgOverlayEntity = entity;
        });
    }

    draw() {
        ECS.forEachEntity(["Sprite"], (entity, sprite) => {
            const {position: {x, y}, spriteIndex, spriteSheet, flipX, origin: {x: ox, y: oy}} = sprite;

            if (ECS.getComponent(entity, "Player")) {
                const scale = getScale(y);
                Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, scale, ox, oy);
            } else {
                Graphics.drawSprite(x, y, spriteIndex, spriteSheet, flipX);
            }
        });

        ECS.forEachEntity(["Sprite"], (entity, sprite) => {
            if (ECS.getComponent(entity, "BackgroundOverlay") || ECS.getComponent(entity, "Player")) {
                return;
            }

            const {position: {x, y}, spriteIndex, spriteSheet, flipX, origin: {x: ox, y: oy}} = sprite;

            Graphics.drawSprite(x, y, spriteIndex, spriteSheet, flipX);
        });

        // More generalized solution requires sorting the entities by origin Y

        if (!this.playerEntity || !this.bgOverlayEntity) {
            return;
        }

        const playerSprite = ECS.getComponent(this.playerEntity, "Sprite");
        const backgroundOverlaySprite = ECS.getComponent(this.bgOverlayEntity, "Sprite");

        // Get player origin Y in the world space
        const {
            position: {x, y},
            origin: {x: ox, y: oy},
            size: {height},
            flipX,
            spriteIndex,
            spriteSheet
        } = playerSprite; // Position is always top-left corner of the sprite. Origin is from 0 to 1, where 0.5 is the center

        // We need to calculate the actual position of the origin in the world. And let's not forget about scale
        const scale = getScale(y);
        const originY = y + height * oy * scale;

        // Calculate origin Y in the world space for the background overlay
        const {position: {y: bgY}, origin: {y: bgOy}, size: {height: bgHeight}} = backgroundOverlaySprite;
        const bgOriginY = bgY + bgHeight * bgOy;

        // Draw them in the correct order. If player's Y is greater than background overlay's Y, draw player first
        if (originY > bgOriginY) {
            Graphics.drawSprite(backgroundOverlaySprite.position.x, bgY, backgroundOverlaySprite.spriteIndex, backgroundOverlaySprite.spriteSheet, backgroundOverlaySprite.flipX);
            Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, scale, ox, oy);
        } else {
            Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, scale, ox, oy);
            Graphics.drawSprite(backgroundOverlaySprite.position.x, bgY, backgroundOverlaySprite.spriteIndex, backgroundOverlaySprite.spriteSheet, backgroundOverlaySprite.flipX);
        }
    }
}

export const spriteDrawSystem = new SpriteDrawSystem();