import {RenderLayer} from "./draw.system";
import {particlesSystem} from "./particles.system";
import {MiasmaParticle, ParticlesEmitter} from "../particles";
import {state} from "../state/state";
import {messageSystem} from "./messageSystem";

// This should not be here
const createMiasmaEmitter = (position) => {
    return new ParticlesEmitter("Miasma Emitter", position, 5, 10, {x: 0, y: 2}, 2, MiasmaParticle);
}

const levels = [
    {
        id: "level1",
        objects: [
            {
                type: "background",
                spriteSheet: 1,
                spriteIndex: 0,
                position: { x: 0, y: 0 },
                size: { width: 320, height: 200 },
                origin: {x: 0, y: 0}
            },
            {
                type: "sprite",
                spriteSheet: 4,
                spriteIndex: 0,
                origin: {x: 0.5, y: 0.75},
                size: {width: 112, height: 29},
                position: { x: 157, y: 138 }
            },
            {
                type: "music",
                musicId: Sound.loadSound('resources/music-1.mp3'),
                volume: 0.5
            },
            {
                type: "particleEmitter",
                subType: "miasma",
                positions: [
                    {x: 261, y: 164},
                    {x: 302, y: 171},
                    {x: 239, y: 202},
                    {x: 66, y: 155},
                    {x: 91, y: 201},
                    {x: 66, y: 215},
                    {x: 16, y: 145},
                    {x: 320 / 2, y: 200}
                ]
            }
        ],
        playerStartPosition: { x: 112, y: 167 },
        playerFacing: "right",
        playerNavMeshIndex: 0,
        playerScale: {
            min: 0.2,
            max: 1,
            quarterScreenMin: 1.0
        },
        message: "Am I wandering through a memory or a dream?#Every flower whispers a story I thought I'd forgotten."
    },
    {
        id: "level2",
        objects: [
            {
                type: "background",
                spriteSheet: 5,
                spriteIndex: 0,
                position: { x: 0, y: 0 },
                size: { width: 320, height: 200 },
                origin: {x: 0, y: 0}
            },
            {
                type: "sprite",
                spriteSheet: 6,
                spriteIndex: 0,
                origin: {x: 0.5, y: 0.85},
                size: {width: 159, height: 118},
                position: { x: 161, y: 206 }
            },
            {
                type: "music",
                musicId: Sound.loadSound('resources/music-2.wav'),
                volume: 0.5
            },
            {
                type: "interactive",
                colliderId: 1,
                description: "A bookshelf filled with books",
                action: {
                    type: "showMessage",
                    text: "Many times I lost myself in these pages...#But today, they seem to be written in a language#I can't understand."
                }
            },
            {
                type: "interactive",
                colliderId: 2,
                description: "A picture of a man",
                action: {
                    type: "showMessage",
                    text: "I painted Miguel two years ago.#Even though we're no longer together,#I can't bring myself to take his portrait down."
                }
            },
            {
                type: "interactive",
                colliderId: 3,
                description: "A plant",
                action: {
                    type: "showMessage",
                    text: "I brought home this aloe to symbolize my#journey of healing.#Now, its leaves wither and fade."
                }
            }
        ],
        playerStartPosition: { x: 38, y: 197 },
        playerFacing: "right",
        playerNavMeshIndex: 1,
        playerScale: {
            min: 0.2,
            max: 1.5,
            quarterScreenMin: 1.5
        },
        message: "This has always been my sanctuary...#But today, it feels like it's shifting beneath my feet."
    }
]

class LevelSystem {
    constructor() {
        this.levelControllerEntity = ECS.createEntity();

        ECS.addComponent(this.levelControllerEntity, "Persistent", {});
        ECS.addComponent(this.levelControllerEntity, "LevelController", {
            loadLevel: null,
            currentLevel: null
        })
    }

    update(deltaTime) {
        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");

        if (levelController.loadLevel) {
            this.loadLevel(levelController.loadLevel);
            levelController.loadLevel = null;
        }
    }

    loadLevel(levelId) {
        log(`Loading level ${levelId}`);

        this.unloadCurrentLevel();

        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        levelController.currentLevel = levelId;

        // Here would be nice to have a transition effects

        this.loadLevelObjects(levelId);
    }

    loadLevelObjects(levelId) {
        const level = levels.find(l => l.id === levelId);
        if (!level) {
            log(`Level ${levelId} not found`);
            return;
        }

        ECS.forEachEntity(["Player", "Sprite", "Animation", "PlayerScale"], (entity, player, sprite, animation, playerScale) => {
            player.position = { ...level.playerStartPosition };
            sprite.position = { ...level.playerStartPosition };
            player.velocity = { x: 0, y: 0 };
            player.path = [];
            player.currentPathIndex = 0;
            animation.currentAnimation = "idle";
            player.navMeshIndex = level.playerNavMeshIndex;
            playerScale.min = level.playerScale.min;
            playerScale.max = level.playerScale.max;
            playerScale.quarterScreenMin = level.playerScale.quarterScreenMin;

            sprite.flipX = level.playerFacing === "left";
        });

        level.objects.forEach(obj => {
            const entity = ECS.createEntity();

            if (obj.type === "background") {
                ECS.addComponent(entity, "Tag", "Background");

                ECS.addComponent(entity, "Sprite", {
                    position: obj.position,
                    spriteIndex: obj.spriteIndex,
                    spriteSheet: obj.spriteSheet,
                    flipX: false,
                    origin: obj.origin,
                    size: obj.size
                });

                ECS.addComponent(entity, "Render", {
                    layer: RenderLayer.Background
                });
            }

            if (obj.type === "sprite") {
                ECS.addComponent(entity, "Sprite", {
                    position: obj.position,
                    spriteIndex: obj.spriteIndex,
                    spriteSheet: obj.spriteSheet,
                    flipX: false,
                    origin: obj.origin,
                    size: obj.size
                });

                ECS.addComponent(entity, "Render", {
                    layer: RenderLayer.Entities
                });
            }

            if (obj.type === "music") {
                ECS.addComponent(entity, "Music", {
                    musicId: obj.musicId
                });

                Sound.playSound(obj.musicId);
                Sound.setSoundVolume(obj.musicId, obj.volume);
            }

            if (obj.type === "particleEmitter") {
                if (obj.subType === "miasma") {
                    obj.positions.forEach(position => {
                        particlesSystem.addEmitter(createMiasmaEmitter(position));
                    });
                }
            }

            if (obj.type === "interactive") {
                ECS.addComponent(entity, "InteractiveObject", {
                    colliderId: obj.colliderId,
                    description: obj.description,
                    action: obj.action
                });
                ECS.addComponent(entity, "Collider", Collision.getCollider(obj.colliderId));
                ECS.addComponent(entity, "Visible", true);
            }
        });

        messageSystem.setMessageText(level.message);
    }

    unloadCurrentLevel() {
        particlesSystem.clear();
        messageSystem.clear();

        ECS.forEachEntity([], (entity) => {
            if (ECS.getComponent(entity, "Music")) {
                Sound.stopSound(ECS.getComponent(entity, "Music").musicId);
            }

            if (ECS.getComponent(entity, "Sound")) {
                const soundComponent = ECS.getComponent(entity, "Sound");
                for (const soundKey in soundComponent.sounds) {
                    const soundData = soundComponent.sounds[soundKey];

                    Sound.stopSound(soundData.soundId);
                    soundData.isStopping = false;
                }
            }

            if (ECS.getComponent(entity, "Light")) {
                // TODO: investigate why this is not working
                // Lighting.removeLightEffect(ECS.getComponent(entity, "Light").name);
            }

            if (!ECS.getComponent(entity, "Persistent")) {
                ECS.destroyEntity(entity);
            }
        });
    }
}

export const levelSystem = new LevelSystem();
