import {messageSystem} from "./messageSystem.js";
import {levelSystem} from "./level.system.js";

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

        const componentTypes = ["InteractiveObject", "Collider"];
        if (levelSystem.isInScene()) {
            componentTypes.push("Scene");
        }

        if (event.type === "mouseDown") {
            const {x, y} = event.position;
            ECS.forEachEntity(componentTypes, (entity, interactiveObject, collider) => {
                const isMouseDown = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);

                if (isMouseDown && interactiveObject.actions) {
                    this.processActions(entity, interactiveObject.actions);
                } else {

                }
            });
        } else if (event.type === "mouseMove") {
            const {x, y} = event.position;

            ECS.forEachEntity(componentTypes, (entity, interactiveObject, collider) => {
                const isHovered = Collision.checkCollisionPoint(x, y, interactiveObject.colliderId);
                if (isHovered) {
                    this.showHoverActions(entity, interactiveObject, {x, y});
                } else {
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

        if (this.hoverEntity) {
            return;
        }

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
        if (this.hoverEntity !== entity) {
            return;
        }

        const actionMenu = ECS.getComponent(this.menu, "ActionMenu");
        if (!actionMenu.hovered) {
            actionMenu.visible = false;
            this.hoverEntity = null;
        }
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
            case "showOverlayScene": {
                levelSystem.showOverlayScene(action.sceneId);
                break;
            }
            case "closeOverlayScene": {
                levelSystem.hideOverlayScene();
                break;
            }
            default:
                log(`Action not implemented: ${action.type}`);
        }
    }

    clear() {
        ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {
            actionMenu.visible = false;
            actionMenu.hovered = false;
        });

        this.hoverEntity = null;
    }
}

export const interactiveObjectsSystem = new InteractiveObjectsSystem();