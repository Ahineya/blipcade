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
                    state.mouseButtonStates[0] = 'up';

                    if (interactiveObject.action) {
                        this.processAction(entity, interactiveObject.action);
                    } else if (interactiveObject.actions) {
                        for (const action of interactiveObject.actions) {
                            this.processAction(entity, action);
                        }
                    }
                }
            }
        });
    }

    processAction(entity, action) {
        switch (action.type) {
            case "showMessage":
                messageSystem.setMessageText(action.text);
                break;
            case "playSound": {
                let found = false;
                ECS.forEachEntity(["ToggleSound"], (entity, toggleSoundComponent) => {
                    if (toggleSoundComponent.soundId === action.soundId) {
                        toggleSoundComponent.shouldToggle = true;
                        found = true;
                    }
                });

                if (!found) {
                    const sound = ECS.createEntity();
                    ECS.addComponent(sound, "ToggleSound", {
                        soundId: action.soundId,
                        volume: action.volume || 1.0,
                        isPlaying: false,
                        shouldToggle: true,
                    });
                }

                break;
            }
            default:
                log(`Action not implemented: ${action.type}`);
        }
    }

}

export const interactiveObjectsSystem = new InteractiveObjectsSystem();