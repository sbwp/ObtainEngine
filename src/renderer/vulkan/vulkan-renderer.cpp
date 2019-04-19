#include "vulkan-renderer.hpp"

#include <vector>
#include <iostream>

#include "instance.hpp"
#include "validation.hpp"
#include "device.hpp"
#include "queue-family-indices.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	VulkanRenderer::VulkanRenderer(
			std::string gameTitle,
			std::array<uint32_t, 3> gameVersion,
			std::array<uint32_t, 3> engineVersion
	)
		:
		gameVersion(gameVersion),
		gameTitle(gameTitle),
		resizeOccurred(false) {

		windowSize = {1600, 900};
		initWindow();

		instance = Instance::createVkInstance(
			gameTitle,
			gameVersion,
			engineVersion
		);
		loader.init(*instance);
		debugMessenger = Validation::createDebugMessenger(
			instance,
			loader
		);

		surface = Device::createSurface(
			*instance,
			window
		);
		physicalDevice = Device::selectPhysicalDevice(
			instance,
			surface
		);
		device = Device::createLogicalDevice(
			instance,
			physicalDevice,
			surface
		);

		indices = QueueFamilyIndices::findQueueFamilies(
			*physicalDevice,
			surface
		);
		graphicsQueue = device->getQueue(
			indices.graphicsFamily.value(),
			0
		);
		presentationQueue = device->getQueue(
			indices.presentFamily.value(),
			0
		);

		createCommandPool();

		swapchain = new Swapchain(
			instance,
			physicalDevice,
			device,
			surface,
			windowSize,
			indices,
			commandPool
		);

	}

	VulkanRenderer::~VulkanRenderer() {
		delete (swapchain);
		instance->destroyDebugUtilsMessengerEXT(
				debugMessenger,
				nullptr,
				loader
		);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VulkanRenderer::run() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			drawFrame();
			bool drawSuccess = swapchain->submitFrame(graphicsQueue, presentationQueue);
			if (resizeOccurred || !drawSuccess) {
				updateWindowSize();
			}
		}

		device->waitIdle();
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto p_resizeOccurred = reinterpret_cast<bool*>(glfwGetWindowUserPointer(window));
		*p_resizeOccurred = true;
	}

	void VulkanRenderer::initWindow() {
		glfwInit();
		glfwWindowHint(
				GLFW_CLIENT_API,
				GLFW_NO_API
		); // Don't create OpenGL context
		glfwWindowHint(
				GLFW_RESIZABLE,
				true
		); // Temp: make window not resizable
		window = glfwCreateWindow(
				windowSize[0],
				windowSize[1],
				gameTitle.c_str(),
				nullptr,
				nullptr
		);
		glfwSetWindowUserPointer(window, &resizeOccurred);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void VulkanRenderer::drawFrame() {

	}

	void VulkanRenderer::createCommandPool() {
		commandPool = device->createCommandPoolUnique(
			vk::CommandPoolCreateInfo(
				vk::CommandPoolCreateFlags(),
				QueueFamilyIndices::findQueueFamilies(
					*physicalDevice,
					surface
				).graphicsFamily.value()
			)
		);
	}

	void VulkanRenderer::updateWindowSize() {
		int width = 0, height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		windowSize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		delete(swapchain);
		swapchain = new Swapchain(
			instance,
			physicalDevice,
			device,
			surface,
			windowSize,
			indices,
			commandPool
		);
		resizeOccurred = false;
	}
}
