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

    namespace Graphics {
        /**
         * Sets the color to use as transparent when drawing.
         */
        function setTransparentColor(color?: number): void;

        /**
         * Fills the screen with a color.
         */
        function fillScreen(color?: number): void;

        /**
         * @descriotion Draws a sprite on the canvas.
         */
        function drawSprite(x: number, y: number, spriteIndex: number, spriteSheetIndex?: number, flipX?: boolean, flipY?: boolean): void;

        /**
         * @descriotion Draws a sprite on the canvas.
         */
        function drawSpriteEx(x: number, y: number, spriteIndex: number, spriteSheetIndex?: number, flipX?: boolean, flipY?: boolean, scale?: number, originX?: number, originY?: number): void;

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
         * Draws a circle on the canvas.
         */
        function drawCircle(center_x: number, center_y: number, radius: number, color: number, params: object): void;

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
