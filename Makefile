LIBS := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))/libs
VULKAN_SDK_PATH = $(LIBS)/vulkansdk/x86_64
HEADER_ONLY_INCLUDE_PATH = $(LIBS)
CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -I$(HEADER_ONLY_INCLUDE_PATH)
LDFLAGS = -g -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
SRC = $(wildcard src/*.cpp) \
	  $(wildcard src/renderer/*.cpp)
OBJ = $(addprefix intermediate/,$(SRC:.cpp=.o))
SHADERS = $(wildcard src/renderer/shaders/shader.*)
SPV = $(addprefix build/shaders/,$(addsuffix .spv,$(subst .,,$(suffix $(SHADERS)))))

build/game: $(OBJ) $(SPV)
	mkdir -p ./build/textures
	cp -R ./src/renderer/textures/* ./build/textures/
	g++ -o $@ $(OBJ) $(LDFLAGS)

intermediate/%.o: %.cpp
	mkdir -p ./intermediate/src/renderer
	g++ -c $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/shaders/%.spv: src/renderer/shaders/shader.%
	mkdir -p ./build/shaders
	$(VULKAN_SDK_PATH)/bin/glslangValidator -V src/renderer/shaders/shader.vert -o build/shaders/vert.spv
	$(VULKAN_SDK_PATH)/bin/glslangValidator -V src/renderer/shaders/shader.frag -o build/shaders/frag.spv

.PHONY: test clean rebuild shaders textures

shaders: $(SPV)

rebuild: clean build/game

test: rebuild
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/explicit_layer.d ./run.sh
	
clean:
	rm -rf build/*
	rm -rf intermediate/*