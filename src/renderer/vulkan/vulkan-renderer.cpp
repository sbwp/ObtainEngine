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
			gameTitle(gameTitle), gameVersion(gameVersion) {
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

		QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(
				physicalDevice,
				*surface
		);
		graphicsQueue = device->getQueue(
				indices.graphicsFamily
				       .value(),
				0
		);
		presentationQueue = device->getQueue(
				indices.presentFamily
				       .value(),
				0
		);

		swapchain = new Swapchain(
				instance,
				physicalDevice,
				device,
				*surface,
				windowSize,
				indices
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
		}
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	void VulkanRenderer::initWindow() {
		glfwInit();
		glfwWindowHint(
				GLFW_CLIENT_API,
				GLFW_NO_API
		); // Don't create OpenGL context
		glfwWindowHint(
				GLFW_RESIZABLE,
				false
		); // Temp: make window not resizable
		window = glfwCreateWindow(
				windowSize[0],
				windowSize[1],
				gameTitle.c_str(),
				nullptr,
				nullptr
		);
	}
}
