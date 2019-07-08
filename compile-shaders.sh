#!/usr/bin/env bash

rsync -ru --delete assets build

if [[ ! -d build/assets/shaders ]]
then
    mkdir -p build/assets/shaders
fi
glslangValidator -V src/graphics/shaders/shader.vert -o build/assets/shaders/vert.spv
glslangValidator -V src/graphics/shaders/shader.frag -o build/assets/shaders/frag.spv