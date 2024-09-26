#!/usr/bin/env bash

cd build-emscripten
emmake cmake .. && emmake cmake --build . && npx serve