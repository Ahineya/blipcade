import {state} from "../state/state.js";
import {messageSystem} from "./messageSystem.js";

class InteractiveObjectsSystem {
    update() {
        ECS.forEachEntity(["InteractiveObject", "Collider"], (entity, interactiveObject, collider) => {
            const {x, y} = Input.getMousePos();
            const isHovered = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);
            if (isHovered) {
                messageSystem.setTooltipText(interactiveObject.description, entity);
            } else {
                messageSystem.clearTooltipText(entity);
            }

            // Check mouse click
            if (state.mouseButtonStates[0] === 'pressed') {
                // const isClicked = Collision.pointInPolygon(mousePosition, collider.vertices); // TODO: implement
                const isClicked = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);
                if (isClicked) {
                    // stopPropagation for the poor:
                    state.mouseButtonStates[0] = 'released';

                    switch (interactiveObject.action.type) {
                        case "showMessage":
                            messageSystem.setMessageText(interactiveObject.action.text);
                            break;
                        default:
                            log(`Action not implemented: ${interactiveObject.action}`);
                    }
                }
            }
        });
    }

}

export const interactiveObjectsSystem = new InteractiveObjectsSystem();