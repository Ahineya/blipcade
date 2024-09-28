//
// Created by Pavlo Yevsehnieiev
//

#include "JsBindings.h"

#include <canvas.h>
#include <codecvt>
#include <iostream>

#include "runtime.h"

namespace blipcade::runtime {
    JSBindings::JSBindings(Runtime &runtime) : m_runtime(runtime) {
    }

    void JSBindings::registerAll(quickjs::value &global) {
        bindLogFunction(global);
        bindTextFunction(global);
        bindGlobalObject(global);
    }

    /**
     * @namespace global
     * @description These are global functions that can be called from the JavaScript code.
     */

    /**
     * @function log
     * @param {string} [message] - The message to log.
     * @description Logs a message to the console.
     */
    void JSBindings::bindLogFunction(quickjs::value &global) {
        global.set_property("log", [this](const std::string &val) {
            std::cout << "log: " << val << std::endl;
        });
    }

    /**
     * @function text
     * @param {string} text - The text to draw.
     * @param {number} x - The x-coordinate of the text.
     * @param {number} [y=0] - The y-coordinate of the text.
     * @param {number} [color=0xfe] - The color of the text (default is 0xfe).
     * @description Draws text on the canvas.
     *
     * @returns {void}
     *
     * @example text("Hello, Blipcade!", 10, 20, 0xff);
     */
    void JSBindings::bindTextFunction(quickjs::value &global) {
        global.set_property("text", [this](const quickjs::args &a) {
            auto argsCount = a.size();

            std::string text;
            int x = 0, y = 0, color = 0xfe;

            if (argsCount >= 1) text = a[0].as_string();
            if (argsCount >= 2) x = a[1].as_int32();
            if (argsCount >= 3) y = a[2].as_int32();
            if (argsCount >= 4) color = a[3].as_int32();

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
            std::wstring wide = converter.from_bytes(text);

            m_runtime.getCanvas()->drawText(*m_runtime.getFont(), wide, x, y, color);
        });
    }

    /**
     * @namespace User
     * @description Provides user-related functionalities.
     */
    void JSBindings::bindGlobalObject(quickjs::value &global) {
        // Create a new object using Object.create(null)
        std::shared_ptr<quickjs::context> ctx = m_runtime.getContext();

        quickjs::value Object = global.get_property("Object");
        quickjs::value obj = Object.call_member("create", quickjs::value::null(*ctx));

        /**
         * @property {string} name - The name of the user.
         */
        obj.set_property("name", quickjs::value(*ctx, "John Doe"));

        /**
         * @function greet
         * @description Greets the user with their name.
         *
         * @example User.greet();
         */
        obj.set_property("greet", quickjs::value(*ctx, [](const quickjs::args &args) -> quickjs::value {
            quickjs::value self = args.get_this();
            quickjs::value name = self.get_property("name");
            return quickjs::value(args.get_context(), "Hello, " + name.as_string() + "!");
        }));
        //
        global.set_property("User", obj);
    }
} // runtime
// blipcade
