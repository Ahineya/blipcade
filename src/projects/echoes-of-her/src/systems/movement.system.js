import {state} from "../state/state.js";
import {levelSystem} from "./level.system.js";

const PLAYER_SPEED = 30; // pixels per second
const WIDTH = 320;
const HEIGHT = 240;

const Screen = {
    width: WIDTH,
    height: HEIGHT
}

const SCREEN_X_SCALE = Screen.width / Screen.height;

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
    return {x: vector.x / length, y: vector.y / length};
}

class MoveSystem {
    update(deltaTime) {
        ECS.forEachEntity(["Player", "Sprite", "Animation", "Sound", "PlayerScale"], (playerEntity, player, sprite, animation, sound, playerScale) => {
            if (player.path && player.currentPathIndex < player.path.length) {
                const targetPoint = player.path[player.currentPathIndex];

                const direction = Geometry.distance(player.position, targetPoint);
                const distanceToTarget = Math.sqrt(direction.x * direction.x + direction.y * direction.y);
                const normalizedDirection = normalize(direction);

                // Calculate the distance the player can move this frame
                const distanceThisFrame = PLAYER_SPEED * getScale(player.position.y, playerScale.min, playerScale.max, playerScale.quarterScreenMin) * (deltaTime / 1000);

                if (distanceThisFrame >= distanceToTarget) {
                    // Move directly to the target point
                    player.position.x = targetPoint.x;
                    player.position.y = targetPoint.y;
                    player.currentPathIndex += 1; // Move to the next point in the path
                } else {
                    // Move towards the target point
                    player.position.x += normalizedDirection.x * distanceThisFrame;
                    player.position.y += normalizedDirection.y * distanceThisFrame;
                }

                // Update sprite position
                sprite.position.x = Math.round(player.position.x);
                sprite.position.y = Math.round(player.position.y);

                // Handle sprite flipping based on movement direction
                if (normalizedDirection.x < 0) {
                    sprite.flipX = true;
                } else if (normalizedDirection.x > 0) {
                    sprite.flipX = false;
                }

                // Handle animations and sounds
                if (normalizedDirection.x !== 0 || normalizedDirection.y !== 0) {
                    animation.currentAnimation = "walk";
                    animation.frameDuration = 300 * getScale(player.position.y);
                    if (!sound.sounds.walk.isPlaying) {
                        sound.sounds.walk.isPlaying = true;
                    }
                } else {
                    animation.currentAnimation = "idle";
                    animation.frameDuration = 700;
                    if (sound.sounds.walk.isPlaying) {
                        sound.sounds.walk.isPlaying = false;
                        sound.sounds.walk.isStopping = true;
                    }
                }

                // Optionally handle collisions here
                // handleCollisions(player, deltaTime); // TODO: Implement this function
            } else {
                // No path to follow; ensure the player stops
                player.velocity.x = 0;
                player.velocity.y = 0;

                animation.currentAnimation = "idle";
                animation.frameDuration = 700;
                sound.sounds.walk.isPlaying = false;
                sound.sounds.walk.isStopping = true;
            }
        });
    }

    handleMouseEvent(event) {
        if (event.type === "mouseDown") {
            if (levelSystem.isInScene()) {
                return;
            }

            const coords = Input.getMousePos();

            ECS.forEachEntity(["Player"], (playerEntity, player) => {
                if (playerEntity) {
                    const currentPosition = player.position;

                    const path = Pathfinding.findPath(
                        Math.round(currentPosition.x),
                        Math.round(currentPosition.y),
                        Math.round(coords.x),
                        Math.round(coords.y),
                        player.navMeshIndex
                    );

                    if (path.length > 0) {
                        player.path = path;
                        player.currentPathIndex = 0;
                    } else {
                        log(`No path found from ${currentPosition.x}, ${currentPosition.y} to ${coords.x}, ${coords.y}`);
                    }
                }
            });
        }
    }
}

// Movement with arrow keys
// class MoveSystem {
//     update(deltaTime) {
//         const velocityX = (() => {
//             switch (true) {
//                 case state.keyStates[1] === 'held':
//                     return -PLAYER_SPEED * SCREEN_X_SCALE;
//                 case state.keyStates[2] === 'held':
//                     return PLAYER_SPEED * SCREEN_X_SCALE;
//                 default:
//                     return 0;
//             }
//         })();
//
//         const velocityY = (() => {
//             switch (true) {
//                 case state.keyStates[4] === 'held':
//                     return -PLAYER_SPEED;
//                 case state.keyStates[8] === 'held':
//                     return PLAYER_SPEED;
//                 default:
//                     return 0;
//             }
//         })();
//
//         ECS.forEachEntity(["Player", "Sprite", "Animation", "Collider", "Sound"], (playerEntity, player, sprite, animation, playerCollider, sound) => {
//             const {position: {y}} = player;
//
//             const scale = getScale(y);
//
//             player.velocity.x = velocityX * scale * SCREEN_X_SCALE;
//             player.velocity.y = velocityY * scale;
//
//             let nextPlayerPositionX = player.position.x + player.velocity.x * (deltaTime / 1000);
//             let nextPlayerPositionY = player.position.y + player.velocity.y * (deltaTime / 1000);
//
//             const {vertices: playerVertices} = playerCollider;
//
//             const withLast = [...playerVertices, playerVertices[0]];
//
//             // Let's transform player playerVertices to the world space
//             const playerWorldVertices = withLast.map((vertice) => {
//                 const start = vertice;
//
//                 // Let's calculate where the line should be drawn in the world based on the player sprite position and origin
//                 const {position: {x, y}, origin: {x: ox, y: oy}, size: {width, height}} = sprite; // Position is always top-left corner of the sprite. Origin is from 0 to 1, where 0.5 is the center
//
//                 // We need to calculate the actual position of the origin in the world. And let's not forget about scale
//                 const scale = getScale(y);
//
//                 const startX = start.x * scale;
//                 const startY = start.y * scale;
//
//                 const originX = x + width * ox * scale;
//                 const originY = y + height * oy * scale;
//
//                 return {x: startX + originX, y: startY + originY};
//             });
//
//             ECS.forEachEntity(["Collider"], (entity, collider) => {
//                 const { vertices } = collider;
//
//                 const worldVertices = vertices.map(({x, y}) => ({x, y}));
//                 worldVertices.push(worldVertices[0]);
//
//                 let minPenetration = Infinity; // Track the smallest penetration
//                 let bestCollisionNormal = null;
//
//                 for (let i = 0; i < playerWorldVertices.length; i++) {
//                     const start = playerWorldVertices[i];
//                     const end = playerWorldVertices[(i + 1) % playerWorldVertices.length];
//
//                     for (let j = 0; j < worldVertices.length; j++) {
//                         const edgeStart = worldVertices[j];
//                         const edgeEnd = worldVertices[(j + 1) % worldVertices.length];
//
//                         const intersect = Geometry.lineIntersect(start, end, edgeStart, edgeEnd);
//
//                         if (intersect) {
//                             let normal = Geometry.perpendicular(edgeStart, edgeEnd);
//                             normal = normalize(normal);
//
//                             const distance = Geometry.distance(start, intersect);
//                             const penetration = Geometry.dot(normal, distance);
//
//                             // Only track the smallest positive penetration value
//                             if (penetration > 0 && penetration < minPenetration) {
//                                 minPenetration = penetration;
//                                 bestCollisionNormal = normal;
//                             }
//                         }
//                     }
//                 }
//
//                 const penetrationMultiplier = 0.7;
//
//                 // Apply the penetration only for the smallest, most significant collision
//                 if (minPenetration !== Infinity && bestCollisionNormal) {
//                     nextPlayerPositionX += bestCollisionNormal.x * minPenetration * penetrationMultiplier;
//                     nextPlayerPositionY += bestCollisionNormal.y * minPenetration * penetrationMultiplier;
//                 }
//             });
//
//             player.position.x = Math.round(Math.max(0, Math.min(nextPlayerPositionX, Screen.width)));
//             player.position.y = Math.round(Math.max(0, Math.min(nextPlayerPositionY, Screen.height)));
//
//             sprite.position.x = player.position.x;
//             sprite.position.y = player.position.y;
//
//             if (player.velocity.x < 0) {
//                 sprite.flipX = true;
//             } else if (player.velocity.x > 0) {
//                 sprite.flipX = false;
//             }
//
//             if (player.velocity.x !== 0 || player.velocity.y !== 0) {
//                 animation.currentAnimation = "walk";
//                 animation.frameDuration = 300 * scale;
//                 sound.sounds.walk.isPlaying = true; // This will actually continuously play the sound
//             } else {
//                 animation.currentAnimation = "idle";
//                 animation.frameDuration = 700;
//                 sound.sounds.walk.isPlaying = false;
//                 sound.sounds.walk.isStopping = true; // This will actually continuously stop the sound
//             }
//         });
//     }
// }

export function getScale(y, min = 0.2, max = 1, quarterScreenMin = 1.0) {

    // if quarterScreenMin is 0, then we want just a simple linear scale from min to max
    if (quarterScreenMin === 0) {
        const quarterScreenY = Screen.height * 0.85;
        const maxScale = 2;

        // Clamp Y to the screen bounds
        const clampedY = Math.max(0, Math.min(y, Screen.height));

        let scale;

        if (clampedY <= quarterScreenY) {
            // Normalize Y to [0,1]
            const t = clampedY / quarterScreenY;

            // Ease-In Quadratic Function
            const easeInQuad = t * t * t * t;

            // Ease-In Sine Function
            // const easeInSin = 1 - Math.cos((Math.PI / 2) * t);

            scale = min + (max - min) * easeInQuad;
        } else {
            // Scale increases linearly from 1.0 to maxScale as y increases from quarterScreenY to Screen.height
            scale = max + ((clampedY - quarterScreenY) / (Screen.height - quarterScreenY)) * (maxScale - 1.0);
        }

        return Math.max(0.2, Math.min(scale, maxScale));
    }

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

        scale = min + (max - min) * easeInSin;
    } else {
        // Scale increases linearly from 1.0 to maxScale as y increases from quarterScreenY to Screen.height
        scale = quarterScreenMin + ((clampedY - quarterScreenY) / (Screen.height - quarterScreenY)) * (maxScale - 1.0);
    }

    return Math.max(0.2, Math.min(scale, maxScale));
}

export const movementSystem = new MoveSystem();