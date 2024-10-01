//
// Created by Pavlo Yevsehnieiev
//

#ifndef DEVTOOL_H
#define DEVTOOL_H
#include <ECS.h>


namespace blipcade::runtime {
    class Runtime;
}

namespace blipcade::devtool {
    class Devtool {
    public:
        Devtool(runtime::Runtime &runtime);
        ~Devtool();

        void draw();
        bool isActive();
        bool setActive(bool active);

        void setFPS(double FPS);

        void RenderECSInspector() const;


    private:
        bool active = false;

        runtime::Runtime &runtime;

        double FPS = 0;
    };
}



#endif //DEVTOOL_H
