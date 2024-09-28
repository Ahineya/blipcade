#pragma once
#include <quickjs.hpp>

#include "runtime.h"

class Runtime; // Forward declaration

namespace blipcade {
    namespace runtime {
        class JSBindings {
        public:
            explicit JSBindings(Runtime &runtime);

            void registerAll(quickjs::value& global);

        private:
            Runtime& m_runtime;

            void bindLogFunction(quickjs::value& global);
            void bindTextFunction(quickjs::value& global);

            void bindGlobalObject(quickjs::value &global);
        };
    }
}
