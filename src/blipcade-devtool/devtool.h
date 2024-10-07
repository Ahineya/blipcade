//
// Created by Pavlo Yevsehnieiev
//

#ifndef DEVTOOL_H
#define DEVTOOL_H
#include <ECS.h>

#include "polygonEditor.h"
#include "spriteEditor.h"


namespace blipcade::runtime {
    class Runtime;
}

namespace blipcade::devtool {
    class Devtool {
    public:
        Devtool(runtime::Runtime &runtime);
        ~Devtool();

        void init();

        void draw();
        bool isActive();
        bool setActive(bool active);

        void setFPS(double FPS);

        void RenderECSInspector() const;

        void drawObjectRecursive(quickjs::context &ctx, const std::string &prefix, const quickjs::value &object) const;

        void setScale(float scale);
        void setCanvasOffset(const Vector2 &offset);
        void setCanvasSize(const Vector2 &size);

        [[nodiscard]] float getScale() const;
        [[nodiscard]] Vector2 getCanvasOffset() const;
        [[nodiscard]] Vector2 getCanvasSize() const;

    private:
        bool active = false;

        runtime::Runtime &runtime;

        double FPS = 0;

        Vector2 canvasOffset = {0, 0};
        Vector2 canvasSize = {0, 0};
        float scale = 1.0f;

        std::string tagFilter;
        char tagFilterBuffer[256];

        PolygonEditor polygonEditor;
        SpriteEditor spriteEditor;
    };
}



#endif //DEVTOOL_H
