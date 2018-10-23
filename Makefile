VULKAN_SDK_PATH = /home/sabrina/vulkansdk/x86_64
CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
SRC = $(wildcard src/*.cpp) \
	  $(wildcard src/renderer/*.cpp)
OBJ = $(addprefix intermediate/,$(SRC:.cpp=.o))
SHADERS = $(wildcard src/renderer/shaders/shader.*)
SPV = $(addprefix build/shaders/,$(addsuffix .spv,$(subst .,,$(suffix $(SHADERS)))))

build/game: $(OBJ) $(SPV)
	g++ -o $@ $(OBJ) $(LDFLAGS)

intermediate/%.o: %.cpp
	mkdir -p ./intermediate/src/renderer
	g++ -c $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/shaders/%.spv: src/renderer/shaders/shader.%
	mkdir -p ./build/shaders
	$(VULKAN_SDK_PATH)/bin/glslangValidator -V src/renderer/shaders/shader.vert -o build/shaders/vert.spv
	$(VULKAN_SDK_PATH)/bin/glslangValidator -V src/renderer/shaders/shader.frag -o build/shaders/frag.spv

.PHONY: test clean rebuild shaders

shaders: $(SPV)

rebuild: clean build/game

test: rebuild
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/explicit_layer.d build/game

clean:
	rm -rf build/*
	rm -rf intermediate/*