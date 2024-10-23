export const fantasyBedroom = {

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
}