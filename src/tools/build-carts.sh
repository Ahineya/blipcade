#!/usr/bin/env bash

# Runs by cmake, so paths are relative to the build directory

#check if CMAKE_SOURCE_DIR is set
if [ -z ${CMAKE_SOURCE_DIR+x} ]; then
    echo "CMAKE_SOURCE_DIR is unset"
    exit 1
fi

cd ${CMAKE_SOURCE_DIR}/src/carts-js && npm run build && cd ${CMAKE_SOURCE_DIR}/src/carts && python3 build-cart.py testcart.json -o testcart-build.json
