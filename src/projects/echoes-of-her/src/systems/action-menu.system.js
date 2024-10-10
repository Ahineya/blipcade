import {interactiveObjectsSystem} from "./interactive-objects.system.js";

class ActionMenuSystem {
    update() {
        ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {

        });
    }
    draw() {
        ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {
            if (!actionMenu.visible) {
                return;
            }

            const {x, y, title} = actionMenu;
            Graphics.drawFilledRect(x, y - 10, title.length * 5 + 2 + 5, 9, 0x2e);
            Graphics.drawFilledRect(x, y - 10, 1, 9, 0xfe);
            text(title, x + 1 + 5, y - 9, 0xfe);

            //Graphics.drawFilledRect(x, y, 20, 20, 0x2e);
            this.drawButtons(entity, actionMenu);
        });
    }

    drawButtons(entity, actionMenu) {
        const {x, y, actions} = actionMenu;
        actions.forEach((action, index) => {
            const buttonX = x + (index * 12) + 5;
            const buttonY = y;
            Graphics.drawFilledRect(buttonX, buttonY, 12, 12, 0x2e);
            text(action.type[0], buttonX + 1, buttonY + 1, 0xfe);
        });
    }

    handleMouseEvent(event) {
        if (event.type === "mouseMove") {
            ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {
                if (!actionMenu.visible) {
                    return;
                }

                let {x, y} = actionMenu;
                y = y - 10;

                const menuWidth = actionMenu.actions.length * 12 + 5;
                const menuHeight = 12 + 10;
                const isHovered = event.position.x >= x && event.position.x <= x + menuWidth && event.position.y >= y && event.position.y <= y + menuHeight

                actionMenu.hovered = isHovered;
            });
        } else if (event.type === "mouseDown") {
            let handled = false;
            ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {
                if (!actionMenu.hovered) {
                    // actionMenu.visible = false;
                    return;
                }

                const {x, y, actions} = actionMenu;
                actions.forEach((action, index) => {
                    const buttonX = x + (index * 12) + 5;
                    const buttonY = y;

                    // const isClicked = Collision.checkCollisionPoint(buttonX, buttonY, 12, 12);
                    // We don't have Collision.checkCollisionPoint yet, let's do a manual check:
                    const isClicked = event.position.x >= buttonX && event.position.x < buttonX + 12 && event.position.y >= buttonY && event.position.y < buttonY + 12

                    if (isClicked) {
                        // this.processAction(entity, action);
                        log(`Action clicked: ${action.type}`);
                        interactiveObjectsSystem.processActions(actionMenu.entity, action.actions);
                        handled = true;
                    }
                });
            });

            return handled;
        }
    }

    clear() {
        ECS.forEachEntity(["ActionMenu"], (entity, actionMenu) => {
            actionMenu.visible = false;
            actionMenu.hovered = false;
        });
    }
}

export const actionMenuSystem = new ActionMenuSystem();