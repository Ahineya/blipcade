//
// Created by Pavlo Yevsehnieiev
//

#include "keystate.h"


namespace blipcade::runtime {
    bool Keystate::isKeyPressed(Key key) const {
        return m_flags & static_cast<uint8_t>(key);
    }

    void Keystate::setKey(Key key, const bool pressed) {
        m_flags = pressed ? m_flags | static_cast<uint8_t>(key) : m_flags & ~static_cast<uint8_t>(key);
    }

} // runtime
// blipcade