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

// Collision helpers
const Geometry = {
    dot: (a, b) => a.x * b.x + a.y * b.y,
    distance: (a, b) => ({x: b.x - a.x, y: b.y - a.y}),
    perpendicular: (a, b) => ({x: b.y - a.y, y: a.x - b.x}),
    cross: (a, b) => a.x * b.y - a.y * b.x,
    lineIntersect: (a, b, c, d) => {
        const r = Geometry.distance(a, b);
        const s = Geometry.distance(c, d);
        const q = Geometry.distance(a, c);

        const rxs = Geometry.cross(r, s);
        const qxr = Geometry.cross(q, r);

        if (rxs === 0) {
            return null;
        }

        const t = Geometry.cross(q, s) / rxs;
        const u = Geometry.cross(q, r) / rxs;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            return {x: a.x + t * r.x, y: a.y + t * r.y};
        }

        return null;
    },
}

const normalize = (vector) => {
    const length = Math.sqrt(vector.x * vector.x + vector.y * vector.y);
    return { x: vector.x / length, y: vector.y / length };
}

const isClockwise = (vertices) => {
    let sum = 0;
    for (let i = 0; i < vertices.length; i++) {
        const current = vertices[i];
        const next = vertices[(i + 1) % vertices.length];
        sum += (next.x - current.x) * (next.y + current.y);
    }
    return sum > 0; // Clockwise if sum is positive
};

const ensureClockwiseWinding = (vertices) => {
    if (isClockwise(vertices)) {
        vertices.reverse(); // Reverse to make winding clockwise
    }
};

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

        ECS.forEachEntity(["Player", "Sprite", "Animation", "Collider"], (playerEntity, player, sprite, animation, playerCollider) => {
            const {position: {y}} = player;

            const scale = getScale(y);

            player.velocity.x = velocityX * scale * SCREEN_X_SCALE;
            player.velocity.y = velocityY * scale;

            let nextPlayerPositionX = player.position.x + player.velocity.x * (deltaTime / 1000);
            let nextPlayerPositionY = player.position.y + player.velocity.y * (deltaTime / 1000);

            const {vertices: playerVertices} = playerCollider;

            const withLast = [...playerVertices, playerVertices[0]];
            // Let's ensure clockwise winding
            ensureClockwiseWinding(withLast);

            // Let's transform player playerVertices to the world space
            const playerWorldVertices = withLast.map((vertice) => {
                const start = vertice;

                // Let's calculate where the line should be drawn in the world based on the player sprite position and origin
                const {position: {x, y}, origin: {x: ox, y: oy}, size: {width, height}} = sprite; // Position is always top-left corner of the sprite. Origin is from 0 to 1, where 0.5 is the center

                // We need to calculate the actual position of the origin in the world. And let's not forget about scale
                const scale = getScale(y);

                const startX = start.x * scale;
                const startY = start.y * scale;

                const originX = x + width * ox * scale;
                const originY = y + height * oy * scale;

                return {x: startX + originX, y: startY + originY};
            });

            ECS.forEachEntity(["Collider"], (entity, collider) => {
                const { vertices } = collider;
                ensureClockwiseWinding(vertices); // Ensure all colliders have consistent winding

                const worldVertices = vertices.map(({x, y}) => ({x, y}));
                worldVertices.push(worldVertices[0]);

                let maxPenetration = 0;
                let collisionNormal = { x: 0, y: 0 };

                for (let i = 0; i < playerWorldVertices.length; i++) {
                    const start = playerWorldVertices[i];
                    const end = playerWorldVertices[(i + 1) % playerWorldVertices.length];

                    for (let j = 0; j < worldVertices.length; j++) {
                        const edgeStart = worldVertices[j];
                        const edgeEnd = worldVertices[(j + 1) % worldVertices.length];

                        const intersect = Geometry.lineIntersect(start, end, edgeStart, edgeEnd);

                        if (intersect) {
                            let normal = Geometry.perpendicular(edgeStart, edgeEnd);
                            normal = normalize(normal);

                            const distance = Geometry.distance(start, intersect);
                            const penetration = Geometry.dot(normal, distance);

                            if (penetration > maxPenetration && penetration > 0) {
                                maxPenetration = penetration;
                                collisionNormal = normal;
                            }
                        }
                    }
                }

                const penetrationMultiplier = 1.2;

                // Apply the most significant penetration if found
                if (maxPenetration > 0) {
                    nextPlayerPositionX += collisionNormal.x * maxPenetration * penetrationMultiplier;
                    nextPlayerPositionY += collisionNormal.y * maxPenetration * penetrationMultiplier;
                }
            });

            player.position.x = Math.max(0, Math.min(nextPlayerPositionX, Screen.width));
            player.position.y = Math.max(0, Math.min(nextPlayerPositionY, Screen.height));

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

        const playerCollider = Collision.getCollider(0);

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
            size: {width: 26, height: 54},
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

        ECS.addComponent(player, "Collider", playerCollider);

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

        for (let i = 1; i < 8; i++) {
            const wall = ECS.createEntity();
            const wallCollider = Collision.getCollider(i);
            ECS.addComponent(wall, "Collider", wallCollider);
        }
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

                Graphics.drawLine(startX + originX, startY + originY, endX + originX, endY + originY, 0xfe);
            }
        });

        ECS.forEachEntity(["Collider"], (entity, collider) => {
            if (ECS.getComponent(entity, "Player")) {
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
}
