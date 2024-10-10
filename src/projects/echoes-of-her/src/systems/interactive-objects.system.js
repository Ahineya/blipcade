import {messageSystem} from "./messageSystem.js";

class InteractiveObjectsSystem {
    hoverEntity = null;

    constructor() {
        const menu = ECS.createEntity();
        ECS.addComponent(menu, "ActionMenu", {
            x: 0,
            y: 0,
            actions: [],
            entity: null,
            visible: false,
            hovered: false,
            title: ""
        });

        ECS.addComponent(menu, "Tag", "ActionMenu");
        ECS.addComponent(menu, "Persistent", {});
        this.menu = menu;
    }

    handleMouseEvent(event) {
        if (event.type === "mouseDown") {
            const {x, y} = event.position;

            let processed = false;

            ECS.forEachEntity(["InteractiveObject", "Collider"], (entity, interactiveObject, collider) => {
                const isClicked = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);
                if (isClicked) {
                    if (interactiveObject.action) {
                        this.processAction(entity, interactiveObject.action);
                    } else if (interactiveObject.actions) {
                        for (const action of interactiveObject.actions) {
                            this.processAction(entity, action);
                        }
                    }

                    processed = true;
                }
            });

            return processed;
        } else if (event.type === "mouseMove") {
            const {x, y} = event.position;

            ECS.forEachEntity(["InteractiveObject", "Collider"], (entity, interactiveObject, collider) => {
                const isHovered = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);
                if (isHovered) {
                    messageSystem.setTooltipText(interactiveObject.description, entity);
                    this.showHoverActions(entity, interactiveObject, {x, y});

                } else {
                    messageSystem.clearTooltipText(entity);
                    this.hideHoverActions(entity);
                }
            });
        }
    }

    showHoverActions(entity, interactiveObject, {x, y}) {
        const hoverActions = ECS.getComponent(entity, "HoverActions");
        if (!hoverActions) {
            return;
        }

        // Here we want to display a menu of actions that can be taken when hovering over an object
        // log(`Hover actions: ${hoverActions.actions.map(action => action.type).join(", ")}`);

        const actionMenu = ECS.getComponent(this.menu, "ActionMenu");
        if (!actionMenu.visible) {
            actionMenu.x = x;
            actionMenu.y = y;
            actionMenu.actions = hoverActions.actions;
            actionMenu.entity = entity;
            actionMenu.visible = true;
            actionMenu.title = interactiveObject.description;
        }

        this.hoverEntity = entity;
    }

    hideHoverActions(entity) {
        // log(`Hide hover actions for entity ${entity}, hoverEntity: ${this.hoverEntity}`);
        if (this.hoverEntity !== entity) {
            return;
        }

        const actionMenu = ECS.getComponent(this.menu, "ActionMenu");
        if (!actionMenu.hovered) {
            actionMenu.visible = false;
        }

        this.hoverEntity = null;


        log("Hide hover actions");
    }

    processActions(entity, actions) {
        actions.forEach(action => {
            this.processAction(entity, action);
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
            case "nextAnimation": {
                const animation = ECS.getComponent(entity, "Animation");

                const animations = Object.keys(animation.animations);
                const currentAnimationIndex = animations.indexOf(animation.currentAnimation);
                const nextAnimationIndex = (currentAnimationIndex + 1) % animations.length;
                animation.currentAnimation = animations[nextAnimationIndex];

                break;
            }
            default:
                log(`Action not implemented: ${action.type}`);
        }
    }

}

export const interactiveObjectsSystem = new InteractiveObjectsSystem();