//
// Created by Pavlo Yevsehnieiev
//

#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H


namespace blipcade::devtool {
    class Devtool;

    class SpriteEditor {
    public:
        explicit SpriteEditor(Devtool &devtool);

        void Draw();

        void saveTexture();

        void SetActive(bool active);

        bool IsActive() const;

    private:
        Devtool &devtool;
        bool active;
    };
} // devtool
// blipcade

#endif //SPRITEEDITOR_H
