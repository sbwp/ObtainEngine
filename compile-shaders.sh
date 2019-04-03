#!/usr/bin/env bash
glslangValidator -V src/renderer/shaders/basic.vert -o assets/shaders/vert.spv
glslangValidator -V src/renderer/shaders/basic.frag -o assets/shaders/frag.spv