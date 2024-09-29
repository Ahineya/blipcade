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
        function drawCircle(center_x: number, center_y: number, radius: number, color: number): void;

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

}
