const { config } = require("@swc/core/spack");

module.exports = config({
    entry: {
        os: __dirname + "/os/os.js",
        loader: __dirname + "/loader/loader.js",
        devtool: __dirname + "/src/index.js",
        celeste: __dirname + "/celeste/celeste.js",
        testcart: __dirname + "/testcart/testcart.js",
        "ecs-test": __dirname + "/ecs-test/ecs-test.js",
        scene: __dirname + "/scene/scene.js",
    },
    output: {
        path: __dirname + "/lib",
    },
    options: {
        "jsc": {
            "parser": {
                "syntax": "ecmascript",
                "jsx": false,
                "dynamicImport": false,
                "privateMethod": false,
                "functionBind": false,
                "exportDefaultFrom": false,
                "exportNamespaceFrom": false,
                "decorators": false,
                "decoratorsBeforeExport": false,
                "topLevelAwait": false,
                "importMeta": false
            },
            "transform": null,
            "target": "es2022",
            "loose": false,
            "externalHelpers": false,
            "keepClassNames": false
        },
    },
    module: {},
});
