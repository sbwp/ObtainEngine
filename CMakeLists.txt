cmake_minimum_required(VERSION 3.12)
project(obtain)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories("libs")
include_directories("libs/vulkansdk/x86_64/include")
add_executable(obtain src/main.cpp src/renderer/renderer.cpp src/renderer/renderer.hpp src/renderer/vulkan/device.cpp src/renderer/vulkan/device.hpp src/renderer/vulkan/instance.cpp src/renderer/vulkan/instance.hpp src/renderer/vulkan/pipeline.cpp src/renderer/vulkan/pipeline.hpp src/renderer/vulkan/queuefamilyindices.hpp src/renderer/vulkan/shader.cpp src/renderer/vulkan/shader.hpp src/renderer/vulkan/swapchain.cpp src/renderer/vulkan/swapchain.hpp src/renderer/vulkan/swapchainsupportdetails.hpp src/renderer/vulkan/uniformbufferobject.hpp src/renderer/vulkan/validation.cpp src/renderer/vulkan/validation.hpp src/renderer/vulkan/vertex.hpp src/renderer/vulkan/vulkanrenderer.cpp src/renderer/vulkan/vulkanrenderer.hpp)