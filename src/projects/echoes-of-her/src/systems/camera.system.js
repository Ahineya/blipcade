import {getScale} from "./movement.system.js";

function lerp(a, b, t) {
    return a + (b - a) * t;
}

const SHOULD_RETURN_CAMERA = true;

// Define screen width and margins
const screenWidth = 320;
const leftMargin = screenWidth * 0.2;
const rightMargin = screenWidth * 0.8;



// Define the camera smoothing factors
let cameraSpeedAhead = 0.02;  // Speed when moving ahead
let cameraSpeedBack = 0.015;  // Speed when returning back

class CameraSystem {
    init() {
        this.camera = ECS.createEntity();
        ECS.addComponent(this.camera, "Camera", {x: 0, y: 0});
        ECS.addComponent(this.camera, "Tag", "Camera");
        ECS.addComponent(this.camera, "Persistent", {});
    }
    update() {
        const cameraComponent = ECS.getComponent(this.camera, "Camera");

        ECS.forEachEntity(["Player", "PlayerScale"], (entity, player, playerScale) => {
            // Define look-ahead distance
            const scale = getScale(player.position.y, playerScale.min, playerScale.max, playerScale.quarterScreenMin);

            const lookAheadDistance = scale < 0.5 ? 0 : screenWidth * scale / 4;

            // Determine player's movement direction
            let lookAheadOffset = 0;
            if (player.velocity.x > 0) { // Moving right
                lookAheadOffset = lookAheadDistance;
            } else if (player.velocity.x < 0) { // Moving left
                lookAheadOffset = -lookAheadDistance;
            }

            // Calculate desired camera x position with look-ahead
            let desiredCameraX = -player.position.x + screenWidth / 2 + lookAheadOffset;

            // Clamp desired camera position within level bounds first
            ECS.forEachEntity(["LevelController"], (entity, levelController) => {
                const LEVEL_WIDTH = levelController.width;
                // Ensure desiredCameraX does not go beyond left boundary
                desiredCameraX = Math.min(0, desiredCameraX);
                // Ensure desiredCameraX does not go beyond right boundary
                desiredCameraX = Math.max(desiredCameraX, -LEVEL_WIDTH + screenWidth);
            });


            // Calculate player's position relative to the current camera
            const playerScreenX = player.position.x + desiredCameraX;

            // Initialize new camera x position
            let newCameraX = desiredCameraX;



            let isAhead = false;
            if ((player.velocity.x > 0 && newCameraX > cameraComponent.x) ||
                (player.velocity.x < 0 && newCameraX < cameraComponent.x)) {
                // Player is moving, and camera needs to catch up (move ahead)
                isAhead = true;
            }

            if (!isAhead && !SHOULD_RETURN_CAMERA) {
                newCameraX = cameraComponent.x;
            }

            // Adjust camera only if player is beyond margins
            // TODO: This does not work now.
            // if (playerScreenX < leftMargin + lookAheadOffset) {
            //     newCameraX = leftMargin + lookAheadOffset - player.position.x;
            // }
            // else if (playerScreenX > rightMargin + lookAheadOffset) {
            //     newCameraX = rightMargin + lookAheadOffset - player.position.x;
            // }

            const distance = Math.abs(newCameraX - cameraComponent.x);

            const minSpeed = isAhead ? cameraSpeedAhead : cameraSpeedBack;
            const maxSpeed = isAhead ? cameraSpeedAhead : cameraSpeedBack * 2;

            // Adjust camera speed based on distance
            // When close to the target, use a higher speed
            // Determine if the camera should be moving ahead or returning back
            let cameraSpeed = cameraSpeedBack;  // Default to returning back
            cameraSpeed = minSpeed + (maxSpeed - minSpeed) * (1 - Math.min(distance / 100, 1));

            // Apply the new camera position smoothly using lerp
            const threshold = 0.9; // Pixel threshold for snapping
            if (distance < threshold) {
                cameraComponent.x = newCameraX;
            } else {
                cameraComponent.x = lerp(cameraComponent.x, newCameraX, cameraSpeed);
            }

            // Clamp camera to level bounds after interpolation
            cameraComponent.x = Math.min(0, cameraComponent.x);

            ECS.forEachEntity(["LevelController"], (entity, levelController) => {
                const LEVEL_WIDTH = levelController.width;
                cameraComponent.x = Math.max(cameraComponent.x, -LEVEL_WIDTH + screenWidth);
            });

            // Apply the updated camera position
            Graphics.setCamera(cameraComponent.x, cameraComponent.y);
        });
    }
}

export const cameraSystem = new CameraSystem();