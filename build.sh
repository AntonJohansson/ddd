#!/bin/bash


[ ! -d build ] && mkdir build

if [ ! -d external/glfw ]; then
    git submodule update --init
    cmake -S external/glfw -B build -DCMAKE_BUILD_TYPE=Release -DGLFW_USE_WAYLAND=On
    cmake --build build
fi

clang src/main.c -o ddd build/src/libglfw3.a -lGL -lm
