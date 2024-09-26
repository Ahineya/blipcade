#!/usr/bin/env bash

cd build-desktop
cmake .. && cmake --build . && ./blipcade_cmake
