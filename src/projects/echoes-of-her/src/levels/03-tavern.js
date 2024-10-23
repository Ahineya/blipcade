export const tavern = {
    id: "tavern",
    objects: [
        {
            type: "background",
            spriteSheet: "res://spritesheets/town/tavern.json",
            spriteIndex: 0,
            position: {x: 0, y: 0},
            size: {width: 320, height: 240},
            origin: {x: 0, y: 0}
        },
        {
            type: "interactive",
            colliderId: "res://colliders/tavern/door.json",
            description: "Door",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "The door to the outside."
                        }
                    ]
                },
                {
                    type: 'Use',
                    actions: [
                        {
                            type: "movePlayer",
                            faceDirectionAfterMove: "right",
                            endDelay: 1,
                            onEndActions: [
                                {
                                    type: "loadLevel",
                                    levelId: "town-default",
                                    playerStartPosition: {x: 296, y: 211},
                                    playerFacing: "left"
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
    ],
    playerStartPosition: {x: 38, y: 197},
    playerFacing: "right",
    playerNavMeshIndex: "res://navmeshes/tavern.json",
    playerScale: {
        min: 0.8,
        max: 1.7,
        quarterScreenMin: 1.7
    },
    animationOffset: 4,
}