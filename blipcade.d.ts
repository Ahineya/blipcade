// Auto-generated TypeScript declarations
// Do not edit manually.

export {};

declare global {
    /**
     * Logs a message to the console.
     */
    function log(message?: string): void;

    /**
     * Draws text on the canvas.
     */
    function text(text: string, x?: number, y?: number, color?: number): void;

    namespace Blip {
        /**
         * Loads a spritesheet from a file.
         */
        function loadSpritesheet(path: string): void;

        /**
         * Loads a navmesh from a file.
         */
        function loadNavmesh(path: string): void;

    }

    namespace Graphics {
        /**
         * Sets the color to use as transparent when drawing.
         */
        function setTransparentColor(color?: number): void;

        /**
         * Sets the camera position.
         */
        function setCamera(offsetX: number, offsetY: number): void;

        /**
         * Fills the screen with a color.
         */
        function fillScreen(color?: number): void;

        /**
         * Draws a sprite on the canvas.
         */
        function drawSprite(x: number, y: number, spriteIndex: number, spriteSheet?: string, flipX?: boolean, flipY?: boolean): void;

        /**
         * Draws a sprite on the canvas.
         */
        function drawSpriteEx(x: number, y: number, spriteIndex: number, spriteSheet?: string, flipX?: boolean, flipY?: boolean, scale?: number, originX?: number, originY?: number): void;

        /**
         * Puts a pixel on the canvas.
         */
        function putPixel(x: number, y: number, color: number): void;

        /**
         * Draws a line on the canvas.
         */
        function drawLine(x1: number, y1: number, x2: number, y2: number, color: number): void;

        /**
         * Draws a filled circle on the canvas.
         */
        function drawFilledCircle(center_x: number, center_y: number, radius: number, color: number): void;

        /**
         * Draws a filled rectangle on the canvas.
         */
        function drawFilledRect(x: number, y: number, width: number, height: number, color: number): void;

        /**
         * Draws a circle on the canvas.
         */
        function drawCircle(center_x: number, center_y: number, radius: number, color: number, params: object): void;

        /**
         * Sets the postprocessing shader.
         */
        function setPostprocessingShader(shaderPath: string): void;

    }

    namespace Lighting {
        /**
         * Adds a new light effect.
         * Lighting.addLightEffect("sunlight", {
         * tintColor: 0xffe066,
         * opacity: 0.5,
         * maskImagePath: "assets/masks/trapezoid.png"
         * });
         */
        function addLightEffect(name: string, params: AddlighteffectParamsParams): void;

        /**
         * Removes an existing light effect.
         * Lighting.removeLightEffect("sunlight");
         */
        function removeLightEffect(name: string): void;

        /**
         * Updates an existing light effect.
         * Lighting.updateLightEffect("sunlight", {
         * tintColor: 0xffffff,
         * opacity: 0.7
         * });
         */
        function updateLightEffect(name: string, params: UpdatelighteffectParamsParams): void;

        /**
         * Updates the opacity of an existing light effect.
         */
        function changeLightOpacity(name: string, opacity: number): void;

        /**
         * Updates the tint color of an existing light effect.
         */
        function changeLightTintColor(name: string, tintColor: number): void;

    }

    namespace Input {
        /**
         * Checks if a key is currently pressed. A key codes are defined in the `Key` enum.
         */
        function isKeyPressed(key: number): boolean;

        /**
         * Gets the current mouse position.
         */
        function getMousePos(): object;

        function isMouseButtonPressed(button: number): boolean;

    }

    namespace ECS {
        /**
         * Creates a new entity.
         */
        function createEntity(): number;

        /**
         * Destroys an entity.
         */
        function destroyEntity(entity: number): void;

        /**
         * Adds a component to an entity.
         */
        function addComponent(entity: number, typeName: string, component: object): void;

        /**
         * Removes a component from an entity.
         */
        function removeComponent(entity: number, typeName: string): void;

        /**
         * Gets a component from an entity.
         */
        function getComponent(entity: number, typeName: string): object;

        /**
         * Iterates over entities that have all the specified components. Iteration order is guaranteed to be the same as the order in which entities were created.
         */
        function forEachEntity(componentTypes: any[], callback: Function, reverse?: boolean): void;

    }

    namespace Collision {
        /**
         * Gets the collider object at the specified index. It has ['type', 'vertices', 'triangles'] properties.
         */
        function getCollider(resourcePath: string): object;

        /**
         * Checks if a point collides with a collider.
         */
        function checkCollisionPoint(x: number, y: number, resourcePath: string): boolean;

    }

    namespace Pathfinding {
        /**
         * Finds a path from the starting point to the ending point using the specified navigation mesh.
         */
        function findPath(startX: number, startY: number, endX: number, endY: number, navigationMeshPath: string): any[];

        /**
         * Gets the navigation mesh with the specified path.
         */
        function getNavMesh(resourcePath: string): any[];

    }

    namespace Sound {
        /**
         * Loads a sound file.
         */
        function loadSound(path: string): number;

        /**
         * Plays a sound.
         */
        function playSound(soundId: number): void;

        /**
         * Stops a sound.
         */
        function stopSound(soundId: number): void;

        /**
         * Toggles a sound.
         */
        function toggleSound(soundId: number): void;

        /**
         * Sets the volume of a sound.
         */
        function setSoundVolume(soundId: number, volume: number): void;

    }

    interface AddlighteffectParamsParams {
        tintColor: number;
        opacity: number;
        maskImagePath: string;
    }

    interface UpdatelighteffectParamsParams {
        tintColor?: number;
        opacity?: number;
        maskImagePath?: string;
    }

}
