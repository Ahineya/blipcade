//
// Created by Pavlo Yevsehnieiev
//

#include "mousestate.h"

#include <raylib.h>

/*
*class Mousestate {
    public:
        Mousestate() = default;

        ~Mousestate() = default;

        void setButton(MouseButton button, bool pressed);

        void setPos(int x, int y);

        [[nodiscard]] int getX() const;

        [[nodiscard]] int getY() const;

        [[nodiscard]] bool isButtonPressed(MouseButton button) const;

    private:
        int m_x = 0;
        int m_y = 0;
        bool m_buttons[3] = {false, false, false};
    };
 */

namespace blipcade::runtime {
    Mousestate::Mousestate() = default;
    Mousestate::~Mousestate() = default;

    void Mousestate::setPos(int x, int y) {
        m_x = x;
        m_y = y;
    }

    void Mousestate::setButton(MouseButton button, bool pressed) {
        m_buttons[static_cast<int>(button)] = pressed;
    }

    int Mousestate::getX() const {
        return m_x;
    }

    int Mousestate::getY() const {
        return m_y;
    }

    Vector2 Mousestate::getPos() const {
        return {static_cast<float>(m_x), static_cast<float>(m_y)};
    }

    bool Mousestate::isButtonPressed(MouseButton button) const {
        return m_buttons[static_cast<int>(button)];
    }
} // runtime
// blipcade