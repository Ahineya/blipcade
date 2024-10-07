#!/usr/bin/env bash

cd build-emscripten
emcmake cmake .. && emmake cmake --build . && npx serve