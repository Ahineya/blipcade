#pragma once
#include <quickjs.hpp>

#include "runtime.h"

class Runtime; // Forward declaration

namespace blipcade {
    namespace runtime {
        class JSBindings {
        public:
            explicit JSBindings(Runtime &runtime);

            void registerAll(quickjs::value &global, ecs::ECS &ecs);

        private:
            Runtime& m_runtime;

            void bindLogFunction(quickjs::value& global);

            void bindSetTransparentColor(quickjs::value &global);

            void bindTextFunction(quickjs::value& global);

            void bindGraphicsGlobalObject(quickjs::value &global);

            void bindFillScreen(quickjs::value &global);

            void bindPutPixel(quickjs::value &global);

            void bindDrawLine(quickjs::value &global);

            void bindDrawFilledCircle(quickjs::value &global);

            void bindDrawCircle(quickjs::value &global);

            void bindInputGlobalObject(quickjs::value &global);

            void bindInputIsKeyPressed(quickjs::value &global);

            void bindInputGetMousePos(quickjs::value &global);

            void bindInputIsMouseButtonPressed(quickjs::value &global);

            void bindDrawSprite(quickjs::value &global);

            void bindDrawSpriteEx(quickjs::value &global);

            void createNamespace(quickjs::value &global, const std::string &name);

            void bindECSGlobalObject(quickjs::value &global, ecs::ECS &ecs);

            void bindCreateEntity(quickjs::value &global, ecs::ECS &ecs);

            void bindDestroyEntity(quickjs::value &global, ecs::ECS &ecs);

            void bindAddComponent(quickjs::value &global, ecs::ECS &ecs);

            void bindRemoveComponent(quickjs::value &global, ecs::ECS &ecs);

            void bindGetComponent(quickjs::value &global, ecs::ECS &ecs);

            void bindForEachEntity(quickjs::value &global, ecs::ECS &ecs);
        };
    }
}
