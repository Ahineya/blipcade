import {state} from "../state/state.js";

export class MessageSystem {
    tooltipEntityId = null;

    constructor() {
        const messageEntity = ECS.createEntity();
        ECS.addComponent(messageEntity, "Message", {
            x: 20,
            y: 200,
            tile: 86,
            last: 0,
            index: 0,
            text: "",
            off: {x: 8, y: 96},
            display: "typed" // "immediate" or "typed"
        });

        ECS.addComponent(messageEntity, "Tag", "Message");

        ECS.addComponent(messageEntity, "Sound", {
            sounds: {
                hit: {
                    soundId: Sound.loadSound('resources/typewriter.mp3'),
                    volume: 0.5,
                    isPlaying: false,
                    isStopping: false
                }
            }
        });

        ECS.addComponent(messageEntity, "Persistent", {});

        const tooltipEntity = ECS.createEntity();
        ECS.addComponent(tooltipEntity, "Message", {
            x: 20,
            y: 20,
            tile: 86,
            last: 0,
            index: 0,
            text: "",
            off: {x: 8, y: 96},
            display: "immediate"
        });

        ECS.addComponent(tooltipEntity, "Tag", "Tooltip");
        ECS.addComponent(tooltipEntity, "Persistent", {});
    }

    update() {
        ECS.forEachEntity(["Message", "Sound"], (entity, message, sound) => {
            if (message.display === "typed") {
                if (!message.text) {
                    return;
                }

                if (message.index < message.text.length) {
                    message.index += 1; // Speed in letters per frame
                    if (message.index >= message.last + 1) {
                        sound.sounds.hit.isPlaying = true;
                        message.last += 1;
                    } else {
                        sound.sounds.hit.isPlaying = false;
                    }
                }
            }
        });
    }

    reset() {
        ECS.forEachEntity(["Message"], (entity, message) => {
            message.index = 0;
            message.last = 0;
        });
    }

    setMessageText(text) {
        ECS.forEachEntity(["Message", "Tag"], (entity, message, tag) => {
            if (tag === "Message") {
                message.text = text;
                this.reset();
            }
        });
    }

    setTooltipText(text, entity) {
        this.tooltipEntityId = entity;
        ECS.forEachEntity(["Message", "Tag"], (entity, message, tag) => {
            if (tag === "Tooltip") {
                message.text = text;
            }
        });
    }

    clearTooltipText(entity) {
        if (this.tooltipEntityId === entity) {
            ECS.forEachEntity(["Message", "Tag"], (entity, message, tag) => {
                if (tag === "Tooltip") {
                    message.text = "";
                }
            });
        }
    }

    clear() {
        this.clearTooltip();
        this.clearMessage();
    }

    clearMessage() {
        ECS.forEachEntity(["Message", "Tag"], (entity, message, tag) => {
            if (tag === "Message") {
                message.text = "";
            }
        });
    }

    clearTooltip() {
        ECS.forEachEntity(["Message", "Tag"], (entity, message, tag) => {
            if (tag === "Tooltip") {
                message.text = "";
            }
        });
    }

    draw() {
        ECS.forEachEntity(["Message"], (entity, message) => {
            const off = { x: message.x, y: message.y };

            if (message.display === "typed") {

                if (state.mouseButtonStates[0] === 'pressed') {
                    if (!message.text) {
                        return;
                    }

                    if (message.index < message.text.length) {
                        message.index = message.text.length;
                    } else {
                        message.text = "";
                        this.reset();
                    }
                }

                for (let i = 0; i < Math.floor(message.index); i++) {
                    if (off.x === message.x) {
                        Graphics.drawFilledRect(off.x - 5, off.y - 1, 5, 9, 0x2e);
                        Graphics.drawFilledRect(off.x - 5, off.y - 1, 1, 9, 0xfe);
                    }

                    if (message.text[i] !== "#") {
                        Graphics.drawFilledRect(off.x - 1, off.y - 1, 6, 9, 0x2e);
                        text(message.text[i], off.x, off.y, 0xfe);
                        off.x += 5;
                    } else {
                        off.x = message.x;
                        off.y += 8;
                    }
                }
            } else {
                if (!message.text) {
                    return;
                }

                const lines = message.text.split("#");
                lines.forEach((line, index) => {
                    Graphics.drawFilledRect(off.x - 2, off.y + (index * 8) - 1, line.length * 5 + 2, 9, 0x2e);
                    text(line, off.x, off.y + (index * 8), 0xfe);
                });
            }
        });
    }
}

export const messageSystem = new MessageSystem();