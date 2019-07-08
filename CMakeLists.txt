cmake_minimum_required(VERSION 3.12)
project(obtain)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)
include_directories(libs/headeronly)
find_package(Vulkan REQUIRED)

add_custom_command(
        OUTPUT build/assets/shaders/frag.spv
        DEPENDS src/graphics/shaders/shader.frag
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ./compile-shaders.sh
)

add_custom_command(
        OUTPUT build/assets/shaders/vert.spv
        DEPENDS src/graphics/shaders/shader.vert
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ./compile-shaders.sh
)

add_custom_target(shaders ALL DEPENDS build/assets/shaders/frag.spv build/assets/shaders/vert.spv)

add_executable(obtain src/main.cpp
        src/graphics/renderer.cpp src/graphics/renderer.hpp
        src/graphics/vulkan/device.cpp src/graphics/vulkan/device.hpp
        src/graphics/vulkan/queue-family-indices.hpp
        src/graphics/vulkan/shader.cpp src/graphics/vulkan/shader.hpp
        src/graphics/vulkan/swapchain.cpp src/graphics/vulkan/swapchain.hpp
        src/graphics/vulkan/swapchain-support-details.hpp src/graphics/vulkan/uniform-buffer-object.hpp
        src/graphics/vulkan/validation.cpp src/graphics/vulkan/validation.hpp
        src/graphics/vulkan/vertex.hpp src/graphics/vulkan/vertex.hpp
        src/graphics/vulkan/vulkan-renderer.cpp src/graphics/vulkan/vulkan-renderer.hpp
        src/graphics/shaders/shader.frag src/graphics/shaders/shader.vert
        src/graphics/vulkan/object.cpp src/graphics/vulkan/object.hpp
        src/graphics/vulkan/object-registry.cpp src/graphics/vulkan/object-registry.hpp
        src/graphics/vulkan/buffer.cpp src/graphics/vulkan/buffer.hpp
        src/utils/time.cpp src/utils/time.hpp
        src/graphics/vulkan/image.cpp src/graphics/vulkan/image.hpp
        src/graphics/vulkan/command.cpp src/graphics/vulkan/command.hpp
        src/graphics/vulkan/model.cpp src/graphics/vulkan/model.hpp
        src/graphics/vulkan/allocator.cpp src/graphics/vulkan/allocator.hpp
        src/graphics/vulkan/memory.hpp
        src/graphics/vulkan/memory-block.cpp src/graphics/vulkan/memory-block.hpp
        src/graphics/vulkan/memory-type.cpp src/graphics/vulkan/memory-type.hpp)


add_dependencies(obtain shaders)

target_link_libraries(obtain glfw ${GLFW_LIBRARIES})
target_include_directories(obtain PRIVATE Vulkan::Vulkan)
target_link_libraries(obtain Vulkan::Vulkan)