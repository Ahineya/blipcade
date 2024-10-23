export const hub = {
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
                    type: "Go to",
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
};