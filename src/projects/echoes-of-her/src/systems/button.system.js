import {levelSystem} from "./level.system.js";
import {interactiveObjectsSystem} from "./interactive-objects.system.js";

class ButtonSystem {
    update() {

        const componentTypes = ["Button"];
        if (levelSystem.isInScene()) {
            componentTypes.push("Scene");
        }

        ECS.forEachEntity(componentTypes, (entity, button) => {

        });
    }

    draw() {
        const componentTypes = ["Button"];
        if (levelSystem.isInScene()) {
            componentTypes.push("Scene");
        }

        ECS.forEachEntity(componentTypes, (entity, button) => {
            const {position: {x, y}, text: buttonText, hovered} = button;

            const width = buttonText.length * 5 + 2;
            const height = 9;

            Graphics.drawFilledRect(x, y, width, height, 0x2e);

            if (hovered) {
                Graphics.drawFilledRect(x, y, width, height, 0xf3);
            }

            text(buttonText, x + 1, y + 1, 0xfe);
        });
    }

    handleMouseEvent(event) {
        const componentTypes = ["Button"];
        if (levelSystem.isInScene()) {
            componentTypes.push("Scene");
        }

        if (event.type === "mouseDown") {
            let handled = false;
            ECS.forEachEntity(componentTypes, (entity, button) => {
                const {position: {x, y}, text } = button;
                const {x: mouseX, y: mouseY} = event.position;

                const width = text.length * 5 + 2;
                const height = 9;

                if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
                    interactiveObjectsSystem.processActions(entity, button.actions);
                    handled = true;
                }
            });

            return handled;
        } else if (event.type === "mouseMove") {
            ECS.forEachEntity(componentTypes, (entity, button) => {
                const {position: {x, y}, text} = button;
                const {x: mouseX, y: mouseY} = event.position;

                const width = text.length * 5 + 2;
                const height = 9;

                if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
                    button.hovered = true;
                } else {
                    button.hovered = false;
                }
            });
        }
    }
}

export const buttonSystem = new ButtonSystem();