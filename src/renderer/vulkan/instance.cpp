#include <iostream>

#include "instance.hpp"

namespace Obtain::Graphics::Vulkan {
	Instance::Instance(std::string gameTitle, std::array<uint32_t, 3> gameVersion) {
		title = gameTitle;
		version = VK_MAKE_VERSION(gameVersion[0], gameVersion[1], gameVersion[2]);
		initWindow();
		const auto &dlst = vk::DispatchLoaderStatic();
		// Check validation layer support if requested
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// Fill application info struct
		auto appInfo = vk::ApplicationInfo(
			title.c_str(), version, "Obtain Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_1
		);

		// Get number of extensions for instance create info struct
		std::vector<const char*> glfwExtensions = getRequiredExtensions();

		// Fill instance create info struct
		auto createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			enableValidationLayers? static_cast<uint32_t>(validationLayers.size()) : 0,
			enableValidationLayers? validationLayers.data() : nullptr,
			static_cast<uint32_t>(glfwExtensions.size()),
			glfwExtensions.data()
		);

		// Create the instance
		instance = vk::createInstanceUnique(createInfo, allocator);

		// Get number of Vulkan extensions supported
		uint32_t extensionCount = 0;
		vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		// Create a vector of that size to hold the extensions
		std::vector<vk::ExtensionProperties> extensions(extensionCount);

		// Query the extension details
		vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Check for required extensions
		bool anyMissing = false;
		std::cout << "";
		for (const auto& glfwExtension : glfwExtensions) {
			bool found = false;
			for (const auto& extension : extensions) {
				if (std::string(extension.extensionName).compare(std::string(glfwExtension)) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				std::cerr << "\t" << glfwExtension << " missing and required" << std::endl;
				anyMissing = true;
			}
		}
		if (anyMissing) {
			throw std::runtime_error("required extension(s) missing");
		}
		
		auto c_surface = VkSurfaceKHR(surface);
		// Creates Window using GLFW
		if (glfwCreateWindowSurface(*instance, window, nullptr, &c_surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		
		/*** Temporary for testing ***/
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
		exit(0);
		// Leaving out validation layers for now until I get a better feel for vulkan.hpp
		// if (enableValidationLayers) {
		// 	vk::DispatchLoaderDynamic dldy;
		// 	dldy.init(*instance);
		// 	vk::DebugUtilsMessengerCreateInfoEXT debugInfo;
		// 	debugInfo.messageSeverity =
		// 		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
		// 	debugInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		// 							vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		// 							vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
		// 	debugInfo.pfnUserCallback = debugCallback;
		// 	vk::DebugUtilsMessengerEXT debugReport = instance.createDebugUtilsMessengerEXT(debugInfo, allocator, dldy);
		// 	device = physicalDevice.createDevice(..., allocator, dldy);
		// 	dldy.init(*device);
		// }
	}
	
	Instance::~Instance() {
		// if (enableValidationLayers) {
		// 	auto func = (vk::destroyDebugReportCallbackEXT)vk::getInstanceProcAddr(*instance, "vkDestroyDebugReportCallbackEXT");
		// 	if (func != nullptr) {
		// 		func(instance, callback, pAllocator);
		// 	}
		// }
	}
	
	void Instance::initWindow() {
		// Initialize GLFW
		glfwInit();

		// Tell GLFW not to create an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Create the actual window
		window = glfwCreateWindow(screenDimensions[0], screenDimensions[1], "Vulkan", nullptr, nullptr);

		// GLFW lets you store an arbitrary pointer, so we point to this VulkanRenderer
		glfwSetWindowUserPointer(window, this);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
	
	// Gets required extensions
	std::vector<const char*> Instance::getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;
	}
	
	// Check Support for Validation Layers
	bool Instance::checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<vk::LayerProperties> availableLayers(layerCount);
		vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
	
	// VKAPI_ATTR bool VKAPI_CALL Instance::debugCallback(
	// 	vk::DebugReportFlagsEXT flags,
	// 	vk::DebugReportObjectTypeEXT objType,
	// 	uint64_t obj,
	// 	size_t location,
	// 	int32_t code,
	// 	const char* layerPrefix,
	// 	const char* msg,
	// 	void* userData
	// ) {

	// 	std::cerr << "validation layer: " << msg << std::endl;

	// 	return false;
	// }

	void Instance::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Instance*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
}