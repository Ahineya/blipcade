#!/usr/bin/env bash

cd build-emscripten
emmake cmake .. && emmake cmake --build . && emrun ./blipcade_cmake.html