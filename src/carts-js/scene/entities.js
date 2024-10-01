import {state} from "./state/state";

const PLAYER_SPEED = 30; // pixels per second

const WIDTH = 320;
const HEIGHT = 240;

const Screen = {
    width: WIDTH,
    height: HEIGHT
}

const SCREEN_X_SCALE = Screen.width / Screen.height;

function getScale(y) {
    const quarterScreenY = Screen.height * 0.65;
    const maxScale = 2;

    // Clamp Y to the screen bounds
    const clampedY = Math.max(0, Math.min(y, Screen.height));

    let scale;

    if (clampedY <= quarterScreenY) {
        // Normalize Y to [0,1]
        const t = clampedY / quarterScreenY;

        // Ease-In Quadratic Function
        // const easeInQuad = t * t;

        // Ease-In Sine Function
        const easeInSin = 1 - Math.cos((Math.PI / 2) * t);

        scale = 0.2 + 0.8 * easeInSin;
    } else {
        // Scale increases linearly from 1.0 to maxScale as y increases from quarterScreenY to Screen.height
        scale = 1.0 + ((clampedY - quarterScreenY) / (Screen.height - quarterScreenY)) * (maxScale - 1.0);
    }

    return Math.max(0.2, Math.min(scale, maxScale));
}

export class MoveSystem {
    update(deltaTime) {
        const velocityX = (() => {
            switch (true) {
                case state.keyStates[1] === 'held':
                    return -PLAYER_SPEED * SCREEN_X_SCALE;
                case state.keyStates[2] === 'held':
                    return PLAYER_SPEED * SCREEN_X_SCALE;
                default:
                    return 0;
            }
        })();

        const velocityY = (() => {
            switch (true) {
                case state.keyStates[4] === 'held':
                    return -PLAYER_SPEED;
                case state.keyStates[8] === 'held':
                    return PLAYER_SPEED;
                default:
                    return 0;
            }
        })();

        ECS.forEachEntity(["Player", "Sprite", "Animation"], (_, player, sprite, animation) => {
            const {position: {y}} = player;

            const scale = getScale(y);

            player.velocity.x = velocityX * scale * SCREEN_X_SCALE;
            player.velocity.y = velocityY * scale;

            player.position.x += player.velocity.x * (deltaTime / 1000);
            player.position.y += player.velocity.y * (deltaTime / 1000);

            sprite.position.x = player.position.x;
            sprite.position.y = player.position.y;

            if (player.velocity.x < 0) {
                sprite.flipX = true;
            } else if (player.velocity.x > 0) {
                sprite.flipX = false;
            }

            if (player.velocity.x !== 0 || player.velocity.y !== 0) {
                animation.currentAnimation = "walk";
                animation.frameDuration = 300 * scale;
            } else {
                animation.currentAnimation = "idle";
                animation.frameDuration = 700;
            }
        });
    }
}

export class Entities {
    constructor() {
        const background = ECS.createEntity();
        ECS.addComponent(background, "Sprite", {
            position: {x: 0, y: 0},
            spriteIndex: 0,
            spriteSheet: 1,
            flipX: false,
            origin: {x: 0, y: 0},
        });

        ECS.addComponent(background, "Tag", "Background");

        const player = ECS.createEntity();

        ECS.addComponent(player, "Tag", "Player");

        ECS.addComponent(player, "Player", {
            position: {x: 112, y: 167},
            velocity: {x: 0, y: 0},
            acceleration: {x: 0, y: 0},
        });

        ECS.addComponent(player, "Sprite", {
            position: {x: 112, y: 167},
            spriteIndex: 0,
            spriteSheet: 2,
            flipX: false,
            origin: {x: 0.5, y: 1},
        });

        ECS.addComponent(player, "Animation", {
            animations: {
                idle: [0, 1],
                walk: [0, 1],
            },
            spritesheets: {
                idle: 2,
                walk: 3,
            },
            currentAnimation: "idle",
            currentFrame: 0,
            frameTime: 0,
            frameDuration: 700,
        });

        const light = ECS.createEntity();
        ECS.addComponent(light, "Light", {
            color: 0xF5F5B2,
            intensity: 1,
        });

        Lighting.addLightEffect("sunlight", {
            tintColor: 0xF5F5B2, // Warm yellow
            opacity: 0.5,
            maskImagePath: "resources/trapezoid.png"
        });

    }

    update(deltaTime) {
        ECS.forEachEntity(["Animation", "Sprite"], (_, animation, sprite) => {
            animation.frameTime += deltaTime; // delta time in ms

            if (animation.frameTime >= animation.frameDuration) {
                animation.frameTime = 0;
                animation.currentFrame++;

                if (animation.currentFrame >= animation.animations[animation.currentAnimation].length) {
                    animation.currentFrame = 0;
                }
            }

            sprite.spriteIndex = animation.animations[animation.currentAnimation][animation.currentFrame];
            sprite.spriteSheet = animation.spritesheets[animation.currentAnimation];
        });

        ECS.forEachEntity(["Light"], (_, light) => {
            light.intensity = (0.5 + 0.5 * Math.sin(Date.now() / 1000)) * 0.5;
            Lighting.changeLightOpacity("sunlight", light.intensity);
        });

        // We want light to slowly fade in and out, intensity should change between 0 and 0.5
        // const light = ECS.getComponent(this.lightEntity, "Light");
        // light.intensity = 1;// 0.5 - 0.5 * Math.sin(Date.now() / 1000);
        // Lighting.changeLightOpacity("sunlight", light.intensity);
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
    }
}
