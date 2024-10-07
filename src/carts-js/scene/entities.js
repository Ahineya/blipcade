import {RenderLayer} from "./systems/draw.system";

export class Entities {
    constructor() {
        // const background = ECS.createEntity();
        // ECS.addComponent(background, "Sprite", {
        //     position: {x: 0, y: 0},
        //     spriteIndex: 0,
        //     spriteSheet: 1,
        //     flipX: false,
        //     origin: {x: 0, y: 0},
        //     size: {width: 320, height: 200},
        // });
        //
        // ECS.addComponent(background, "Render", {
        //     layer: RenderLayer.Background
        // });

        // ECS.addComponent(background, "Tag", "Background");

        const player = ECS.createEntity();
        // const playerCollider = Collision.getCollider(0);

        ECS.addComponent(player, "Tag", "Player");

        ECS.addComponent(player, "Player", {
            position: {x: 112, y: 167},
            velocity: {x: 0, y: 0},
            acceleration: {x: 0, y: 0},
            path: [],
            currentPathIndex: 0,
            navMeshIndex: 0,
        });

        ECS.addComponent(player, "Persistent", {});

        ECS.addComponent(player, "PlayerScale", {
            min: 0.2,
            max: 1,
            quarterScreenMin: 1.5,
        })

        const walkSoundHandle = Sound.loadSound('resources/footsteps.mp3');

        ECS.addComponent(player, 'Sound', {
            sounds: {
                walk: {
                    soundId: walkSoundHandle,
                    volume: 1.0,
                    isPlaying: false,
                    isStopping: false,
                }
            }
        });

        ECS.addComponent(player, "Sprite", {
            position: {x: 112, y: 167},
            spriteIndex: 0,
            spriteSheet: 2,
            flipX: false,
            origin: {x: 0.5, y: 0.8},
            size: {width: 26, height: 54},
        });

        ECS.addComponent(player, "Render", {
            layer: RenderLayer.Entities
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

        // ECS.addComponent(player, "Collider", playerCollider);
        //
        // for (let i = 1; i < 8; i++) {
        //     const wall = ECS.createEntity();
        //     const wallCollider = Collision.getCollider(i);
        //     ECS.addComponent(wall, "Collider", wallCollider);
        //     ECS.addComponent(wall, "Visible", false);
        // }
    }

    update(deltaTime) {

    }
}
