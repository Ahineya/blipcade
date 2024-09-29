class Input_ {
    isButtonPressed(button) {
        // Map keys to buttons
        const keyMap = {
            0: 1, // Left
            1: 2, // Right
            2: 4, // Up
            3: 8, // Down
            4: 16, // Z
            5: 32, // X
        };
        const key = keyMap[button];
        return isKeyPressed(key);
    }
}

export const Input = new Input_();