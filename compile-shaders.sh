#!/usr/bin/env bash
if [[ ! -d build/assets/shaders ]]
then
    mkdir -p build/assets/shaders
fi
glslangValidator -V src/renderer/shaders/basic.vert -o build/assets/shaders/vert.spv
glslangValidator -V src/renderer/shaders/basic.frag -o build/assets/shaders/frag.spv