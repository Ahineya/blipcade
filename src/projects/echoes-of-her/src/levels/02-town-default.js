export const townDefault = {
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
                    type: 'Go to',
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
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/tavern-door.json",
            description: "Tavern door",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "My mom's tavern.#I've spent many hours here."
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
                                    levelId: "tavern",
                                    playerStartPosition: {x: 168, y: 174},
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
            ],
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/view.json",
            description: "Mountains",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "The mountains are beautiful.#Seeing them every day makes me happy."
                        }
                    ]
                }
            ]
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/home.json",
            description: "My home",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "My home.#I've lived here all my life."
                        }
                    ]
                }
            ]
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/tavern.json",
            description: "Tavern",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "My mom's tavern.#I've spent many hours here."
                        }
                    ]
                }
            ],
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/tavern-outdoors-table.json",
            description: "Table",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "My favorite spot.#I love to sit here and watch the town."
                        }
                    ]
                }
            ],
        },
        {
            type: "interactive",
            colliderId: "res://colliders/town-default/tavern-sign.json",
            description: "Tavern sign",
            hoverActions: [
                {
                    type: 'Look at',
                    actions: [
                        {
                            type: "showMessage",
                            text: "The sign of my mom's tavern.#I helped her paint it."
                        }
                    ]
                }
            ],
        },
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
};