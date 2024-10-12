import {RenderLayer} from "./draw.system.js";
import {particlesSystem} from "./particles.system.js";
import {MiasmaParticle, ParticlesEmitter} from "../particles.js";
import {messageSystem} from "./messageSystem.js";
import {actionMenuSystem} from "./action-menu.system.js";
import {interactiveObjectsSystem} from "./interactive-objects.system.js";

// This should not be here
const createMiasmaEmitter = (position) => {
    return new ParticlesEmitter("Miasma Emitter", position, 5, 10, {x: 0, y: 2}, 2, MiasmaParticle);
}

const scenes = [
    {
        id: "level1",
        objects: [
            {
                type: "background",
                spriteSheet: 1,
                spriteIndex: "res://spritesheets/level1-bg.json",
                position: {x: 0, y: 0},
                size: {width: 320, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "sprite",
                spriteSheet: "res://spritesheets/level1-bg-overlay.json",
                spriteIndex: 0,
                origin: {x: 0.5, y: 0.75},
                size: {width: 112, height: 29},
                position: {x: 157, y: 138}
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
                    // {x: 320 / 2, y: 200}
                ]
            }
        ],
        playerStartPosition: {x: 112, y: 167},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/level1.json",
        playerScale: {
            min: 0.2,
            max: 1,
            quarterScreenMin: 1.0
        },
        animationOffset: 0,
        message: "Am I wandering through a memory or a dream?#Every flower whispers a story I thought I'd forgotten."
    },
    {
        id: "fantasy-bedroom",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/fantasy-bedroom.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "spriteGroup",
                sprites: [
                    {
                        type: "sprite",
                        spriteSheet: "res://spritesheets/fantasy-bed.json",
                        spriteIndex: 0,
                        origin: {x: 0.5, y: 0.8},
                        size: {width: 187, height: 129},
                        position: {x: 162, y: 210}
                    },
                    {
                        type: "sprite",
                        spriteSheet: "res://spritesheets/teddy.json",
                        spriteIndex: 0,
                        origin: {x: 0.5, y: 0.5},
                        size: {width: 23, height: 22},
                        position: {x: 180, y: 160}
                    },
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/teddy.json",
                description: "A teddy bear",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "My favorite teddy bear.#I've had it since I was a child."
                            }
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/fantasy-bedroom/plants.json",
                description: "Plants",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "I've always loved plants.#My mother says they bring life to a room."
                            }
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/fantasy-bedroom/wall.json",
                description: "Decorated wall",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "I made these carpets and paintings myself.#I am proud of them."
                            }
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/fantasy-bedroom/door.json",
                description: "A door",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "When I was a child, I used to imagine#that this door led to a magical world."
                            }
                        ]
                    },
                    {
                        type: 'Use',
                        actions: [
                            {
                                type: "movePlayer",
                                faceDirectionAfterMove: "left",
                                endDelay: 7,
                                onEndActions: [
                                    {
                                        type: "loadLevel",
                                        levelId: "town-default",
                                        playerStartPosition: {x: 40, y: 220},
                                        playerFacing: "right"
                                    },
                                    {
                                        type: "playSound",
                                        soundId: Sound.loadSound("res://sounds/door.mp3"),
                                    }
                                ]
                            },
                        ]
                    }
                ]
            }
        ],
        playerStartPosition: {x: 38, y: 197},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/fantasy-bedroom.json",
        playerScale: {
            min: 0.2,
            max: 2.4,
            quarterScreenMin: -1
        },
        animationOffset: 4,
    },
    {
        id: "town-default",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/town/town-default.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "interactive",
                colliderId: "res://colliders/town-default/home-door.json",
                description: "A door",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "This door leads to my home.#I've always felt safe here."
                            }
                        ]
                    },
                    {
                        type: 'Use',
                        actions: [
                            {
                                type: "movePlayer",
                                faceDirectionAfterMove: "left",
                                endDelay: 7,
                                onEndActions: [
                                    {
                                        type: "loadLevel",
                                        levelId: "fantasy-bedroom",
                                        playerStartPosition: {x: 38, y: 197},
                                        playerFacing: "right"
                                    },
                                    {
                                        type: "playSound",
                                        soundId: Sound.loadSound("res://sounds/door.mp3"),
                                    }
                                ]
                            },
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/town-default/town-exit.json",
                description: "Town exit",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "There are very nice views this way."
                            }
                        ]
                    },
                    {
                        type: 'Use',
                        actions: [
                            {
                                type: "movePlayer",
                                faceDirectionAfterMove: "left",
                                endDelay: 1,
                                onEndActions: [
                                    {
                                        type: "loadLevel",
                                        levelId: "hub",
                                        playerStartPosition: {x: 139, y: 140},
                                        playerFacing: "right"
                                    },
                                ]
                            },
                        ]
                    }
                ]
            }
        ],
        playerStartPosition: {x: 38, y: 197},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/fantasy-town.json",
        playerScale: {
            min: 0.2,
            max: 0.6,
            quarterScreenMin: -2
        },
        animationOffset: 4,
    },
    {
        id: "level2-x",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/level2-bg.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320, height: 200},
                origin: {x: 0, y: 0}
            },
            {
                type: "spriteGroup",
                sprites: [
                    {
                        type: "sprite",
                        spriteSheet: "res://spritesheets/level2-bg-overlay.json",
                        spriteIndex: 0,
                        origin: {x: 0.5, y: 0.85},
                        size: {width: 159, height: 118},
                        position: {x: 161, y: 206}
                    },
                    {
                        type: "sprite",
                        spriteSheet: "res://spritesheets/teddy.json",
                        spriteIndex: 0,
                        origin: {x: 0.5, y: 0.5},
                        size: {width: 23, height: 22},
                        position: {x: 180, y: 160}
                    },
                ]
            },
            {
                type: "music",
                musicId: Sound.loadSound('resources/music-2.wav'),
                volume: 0.5
            },
            {
                type: "interactive",
                colliderId: "res://colliders/bookshelf.json",
                description: "A bookshelf",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "Many times I lost myself in these pages...#But today, they seem to be written in a language#I can't understand."
                            }
                        ]
                    }

                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/painting.json",
                description: "A picture of a man",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "I painted Miguel two years ago.#Even though we're no longer together,#I can't bring myself to take his portrait down."
                            }
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/plant.json",
                description: "A plant",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "I brought home this aloe to symbolize my#journey of healing.#Now, its leaves wither and fade."
                            }
                        ]
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/teddy.json",
                description: "A teddy bear",
                hoverActions: [
                    {
                        type: 'Look at',
                        actions: [
                            {
                                type: "showMessage",
                                text: "This teddy bear was my companion#through many sleepless nights."
                            }
                        ]
                    }
                ]
            }
        ],
        playerStartPosition: {x: 38, y: 197},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/level2.json",
        playerScale: {
            min: 0.2,
            max: 1.5,
            quarterScreenMin: 1.5
        },
        animationOffset: 0,
        message: "This has always been my sanctuary...#But today, it feels like it's shifting beneath my feet."
    },
    {
        id: "level3",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/level3-bg.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "interactive",
                colliderId: "res://colliders/photoalbum.json",
                description: "A photo album",
                hoverActions: [
                    {
                        type: "Look at",
                        actions: [
                            {
                                type: "showMessage",
                                text: "This photo album holds memories of#people and places that no longer exist.#I'm afraid to open it."
                            }
                        ]
                    },
                    {
                        type: "Use",
                        actions: [
                            {
                                type: "movePlayer",
                                endDelay: 1,
                                onEndActions: [
                                    {
                                        type: "showOverlayScene",
                                        sceneId: "photoalbum scene"
                                    }
                                ]
                            }
                        ]
                    }
                ],
            },
            {
                type: "interactive",
                colliderId: "res://colliders/radio.json",
                description: "A vintage radio",
                hoverActions: [
                    {
                        type: "Look at",
                        actions: [
                            {
                                type: "showMessage",
                                text: "This radio played the soundtrack of my youth.#Sometimes, I turn it on hoping to hear a melody#that brings back forgotten joys."
                            }
                        ]
                    },
                    {
                        type: "Use",
                        actions: [
                            {
                                type: "movePlayer",
                                faceDirectionAfterMove: "left",
                                endDelay: 3,
                                onEndActions: [
                                    {
                                        type: "playSound",
                                        soundId: Sound.loadSound('res://sounds/radio/searching-for-light-filtered.mp3'),
                                        volume: 0.5
                                    },
                                    {
                                        type: "nextAnimation"
                                    }
                                ]
                            }
                        ]
                    }
                ],
                sprite: {
                    type: "sprite",
                    spriteSheet: "res://spritesheets/radio.json",
                    spriteIndex: 0,
                    origin: {x: 0.5, y: 0.5},
                    size: {width: 35, height: 18},
                    position: {x: 191, y: 159}
                },
                animation: {
                    type: "animation",
                    animations: {
                        idle: [0],
                        playing: [1, 2, 3, 4, 5, 6, 7, 8]
                    },
                    currentAnimation: "idle",
                    spritesheets: {
                        idle: "res://spritesheets/radio.json",
                        playing: "res://spritesheets/radio.json"
                    },
                    currentFrame: 0,
                    frameTime: 0,
                    frameDuration: 700,
                    spriteOffset: 0
                }
            }
        ],
        playerStartPosition: {x: 38, y: 197},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/level3.json",
        playerScale: {
            min: 0.2,
            max: 1.4,
            quarterScreenMin: 1.4
        },
        animationOffset: 0,
    },
    {
        id: "hub",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/hub.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320 * 3, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "sprite",
                spriteSheet: "res://spritesheets/hub-middle-overlay.json",
                spriteIndex: 0,
                origin: {x: 0.5, y: 1.0},
                size: {width: 254, height: 49},
                position: {x: 320 + 160, y: 240}
            },
            {
                type: "music",
                musicId: Sound.loadSound('res://sounds/hub/main.mp3'),
                volume: 0.2
            },
            {
                type: "interactive",
                colliderId: "res://colliders/hub/town.json",
                description: "Town",
                hoverActions: [
                    {
                        type: "Look at",
                        actions: [
                            {
                                type: "showMessage",
                                text: "I've lived here all my life."
                            }
                        ]
                    },
                    {
                        type: "Use",
                        actions: [
                            {
                                type: "movePlayer",
                                faceDirectionAfterMove: "right",
                                endDelay: 1,
                                onEndActions: [
                                    {
                                        type: "loadLevel",
                                        levelId: "town-default",
                                        playerStartPosition: {x: 99, y: 187},
                                        playerFacing: "right"
                                    },
                                ]
                            },
                        ]
                    }
                ],
            }
        ],
        playerStartPosition: {x: 475, y: 187},
        playerFacing: "right",
        playerNavMeshIndex: "res://navmeshes/hub-1.json",
        playerScale: {
            min: 0.1,
            max: 2.5,
            quarterScreenMin: 0
        },
        camera: {x: -320, y: 0},
        animationOffset: 4,
    },
    {
        id: "photoalbum scene",
        type: "overlay",
        objects: [
            {
                type: "background",
                spriteSheet: "res://spritesheets/photoalbum.json",
                spriteIndex: 0,
                position: {x: 0, y: 0},
                size: {width: 320, height: 240},
                origin: {x: 0, y: 0}
            },
            {
                type: "button",
                position: {x: 160, y: 20},
                text: "Close",
                actions: [
                    {
                        type: "closeOverlayScene",
                    }
                ]
            },
            {
                type: "interactive",
                colliderId: "res://colliders/photoalbum/left.json",
                description: "Miguel",
                hoverActions: [
                    {
                        type: "Look at",
                        actions: [
                            {
                                type: "showMessage",
                                text: "Miguel was my second love.#We met in the summer of 1999."
                            }
                        ]
                    }
                ],
            },
            {
                type: "interactive",
                colliderId: "res://colliders/photoalbum/right.json",
                description: "Me",
                hoverActions: [
                    {
                        type: "Look at",
                        actions: [
                            {
                                type: "showMessage",
                                text: "This is me, in the summer of 1999.#I was so young and full of dreams."
                            }
                        ]
                    }
                ],
            }
        ]
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

    showOverlayScene(sceneId) {
        // Load scene json
        // Create a scene entity with a Scene component {sceneId: sceneId}
        // Load scene the same way as levels, but for each object, add a Scene component {sceneId: sceneId}. And ignore player related components
        // draw system should draw the scene on top of the current scene

        const scene = ECS.createEntity();
        ECS.addComponent(scene, "Scene", {
            sceneId
        });

        this.loadLevelObjects(sceneId, {sceneId});
        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        levelController.isInScene = true;

        interactiveObjectsSystem.clear();
    }

    hideOverlayScene() {
        ECS.forEachEntity(["Scene"], (entity, sceneComponent) => {
            ECS.destroyEntity(entity);
        });

        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        levelController.isInScene = false;
    }

    isInScene() {
        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        return levelController.isInScene;
    }

    // TODO: Remove in production
    nextLevel() {
        // We want to filter out the overlay scenes
        const levels = scenes.filter(scene => !scene.type || scene.type !== "overlay");
        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        const currentLevel = levelController.currentLevel;
        const currentLevelIndex = levels.findIndex(l => l.id === currentLevel);

        this.loadLevel(levels[(currentLevelIndex + 1) % levels.length].id);
    }

    loadLevel(levelId, {playerStartPosition, playerFacing} = {}) {
        log(`Loading level ${levelId}`);

        this.unloadCurrentLevel();

        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        levelController.currentLevel = levelId;

        // Here would be nice to have a transition effects

        this.loadLevelObjects(levelId, {playerStartPosition, playerFacing});
    }

    loadLevelObjects(levelId, {sceneId, playerStartPosition, playerFacing} = {}) {
        const level = scenes.find(l => l.id === levelId);
        if (!level) {
            log(`Level ${levelId} not found`);
            return;
        }

        const levelController = ECS.getComponent(this.levelControllerEntity, "LevelController");
        levelController.width = level.objects.find(obj => obj.type === "background").size.width;
        levelController.height = level.objects.find(obj => obj.type === "background").size.height;

        if (!sceneId) {
            ECS.forEachEntity(["Player", "Sprite", "Animation", "PlayerScale"], (entity, player, sprite, animation, playerScale) => {
                player.position = playerStartPosition ? {...playerStartPosition} : {...level.playerStartPosition};
                sprite.position = player.position;
                player.velocity = {x: 0, y: 0};
                player.path = [];
                player.currentPathIndex = 0;
                animation.currentAnimation = "idle";
                player.navMeshIndex = level.playerNavMeshIndex;
                playerScale.min = level.playerScale.min;
                playerScale.max = level.playerScale.max;
                playerScale.quarterScreenMin = level.playerScale.quarterScreenMin;

                sprite.flipX = playerFacing ? playerFacing === "left" : level.playerFacing === "left";

                if (level.animationOffset !== undefined) {
                    animation.spriteOffset = level.animationOffset;
                }
            });
        }

        const cameraOffset = playerStartPosition ? {x: playerStartPosition.x, y: 0} : (level.camera || {x: 0, y: 0});

        ECS.forEachEntity(["Camera"], (entity, camera) => {
            camera.x = cameraOffset.x;
            camera.y = cameraOffset.y;
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
                    layer: obj.layer || RenderLayer.Entities,
                    renderOrder: obj.renderOrder || 0
                });
            }

            if (obj.type === "spriteGroup") {
                ECS.addComponent(entity, "SpriteGroup", {
                    sprites: obj.sprites
                });

                ECS.addComponent(entity, "Render", {
                    layer: obj.layer || RenderLayer.Entities
                })
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
                    action: obj.action,
                    actions: obj.actions
                });
                ECS.addComponent(entity, "Collider", Collision.getCollider(obj.colliderId));
                ECS.addComponent(entity, "Visible", true);

                if (obj.sprite) {
                    ECS.addComponent(entity, "Sprite", obj.sprite);

                    ECS.addComponent(entity, "Render", {
                        layer: RenderLayer.Entities
                    });
                }

                if (obj.animation) {
                    ECS.addComponent(entity, "Animation", obj.animation);
                }

                if (obj.hoverActions) {
                    ECS.addComponent(entity, "HoverActions", {
                        actions: obj.hoverActions
                    });
                }
            }

            if (obj.type === "button") {
                ECS.addComponent(entity, "Button", {
                    position: obj.position,
                    text: obj.text,
                    actions: obj.actions
                });
            }

            if (sceneId) {
                ECS.addComponent(entity, "Scene", {
                    sceneId
                });
            }
        });

        messageSystem.setMessageText(level.message);
    }

    unloadCurrentLevel() {
        particlesSystem.clear();
        messageSystem.clear();
        actionMenuSystem.clear();
        interactiveObjectsSystem.clear();

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

            if (ECS.getComponent(entity, "ToggleSound")) {
                const toggleSoundComponent = ECS.getComponent(entity, "ToggleSound");
                Sound.stopSound(toggleSoundComponent.soundId);
            }

            if (ECS.getComponent(entity, "Light")) {
                Lighting.removeLightEffect(ECS.getComponent(entity, "Light").name);
            }

            if (!ECS.getComponent(entity, "Persistent")) {
                ECS.destroyEntity(entity);
            }
        });
    }
}

export const levelSystem = new LevelSystem();
