//
// Created by Pavlo Yevsehnieiev
//

#ifndef KEYSTATE_H
#define KEYSTATE_H
#include <cstdint>


namespace blipcade::runtime {
    enum class Key {
        Left = 0b00000001,
        Right = 0b00000010,
        Up = 0b00000100,
        Down = 0b00001000,
        A = 0b00010000,
        B = 0b00100000,
        Start = 0b01000000,
        Select = 0b10000000
    };

    class Keystate {
    public:
        Keystate() = default;

        ~Keystate() = default;

        void setKey(Key key, bool pressed);

        [[nodiscard]] bool isKeyPressed(Key key) const;

    private:
        uint8_t m_flags = 0;
    };
} // runtime
// blipcade

#endif //KEYSTATE_H
