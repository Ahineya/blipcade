//
// Created by Pavlo Yevsehnieiev
//

#ifndef MOUSESTATE_H
#define MOUSESTATE_H
#include <raylib.h>

/*
*auto mouse_pos = GetMousePosition();
            runtime->mouseMove(mouse_pos.x, mouse_pos.y);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                runtime->mouseDown(runtime::MouseButton::Left);
            } else {
                runtime->mouseUp(runtime::MouseButton::Left);
            }

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                runtime->mouseDown(runtime::MouseButton::Right);
            } else {
                runtime->mouseUp(runtime::MouseButton::Right);
            }
 */


namespace blipcade::runtime {
    enum class MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    class Mousestate {
    public:
        Mousestate();
        ~Mousestate();

        void setButton(MouseButton button, bool pressed);

        void setPos(int x, int y);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        Vector2 getPos() const;

        [[nodiscard]] bool isButtonPressed(MouseButton button) const;

    private:
        int m_x = 0;
        int m_y = 0;
        bool m_buttons[3] = {false, false, false};
    };
} // runtime
// blipcade

#endif //MOUSESTATE_H
