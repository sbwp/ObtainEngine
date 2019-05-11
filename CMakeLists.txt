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
        DEPENDS src/renderer/shaders/shader.frag
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ./compile-shaders.sh
)

add_custom_command(
        OUTPUT build/assets/shaders/vert.spv
        DEPENDS src/renderer/shaders/shader.vert
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ./compile-shaders.sh
)

add_custom_target(shaders ALL DEPENDS build/assets/shaders/frag.spv build/assets/shaders/vert.spv)

add_executable(obtain src/main.cpp
        src/renderer/renderer.cpp src/renderer/renderer.hpp
        src/renderer/vulkan/device.cpp src/renderer/vulkan/device.hpp
        src/renderer/vulkan/queue-family-indices.hpp
        src/renderer/vulkan/shader.cpp src/renderer/vulkan/shader.hpp
        src/renderer/vulkan/swapchain.cpp src/renderer/vulkan/swapchain.hpp
        src/renderer/vulkan/swapchain-support-details.hpp src/renderer/vulkan/uniform-buffer-object.hpp
        src/renderer/vulkan/validation.cpp src/renderer/vulkan/validation.hpp
        src/renderer/vulkan/vertex.hpp src/renderer/vulkan/vertex.hpp
        src/renderer/vulkan/vulkan-renderer.cpp src/renderer/vulkan/vulkan-renderer.hpp
        src/renderer/shaders/shader.frag src/renderer/shaders/shader.vert
        src/renderer/vulkan/object.cpp src/renderer/vulkan/object.hpp
        src/renderer/vulkan/buffer.cpp src/renderer/vulkan/buffer.hpp
        src/utils/time.cpp src/utils/time.hpp
        src/renderer/vulkan/image.cpp src/renderer/vulkan/image.hpp
        src/renderer/vulkan/command.cpp src/renderer/vulkan/command.hpp src/renderer/vulkan/model.cpp src/renderer/vulkan/model.hpp)


add_dependencies(obtain shaders)

target_link_libraries(obtain glfw ${GLFW_LIBRARIES})
target_include_directories(obtain PRIVATE Vulkan::Vulkan)
target_link_libraries(obtain Vulkan::Vulkan)