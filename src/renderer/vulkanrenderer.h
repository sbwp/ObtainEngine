#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <set>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <algorithm>
#include <fstream>
#include "queuefamilyindices.h"
#include "swapchainsupportdetails.h"

class VulkanRenderer {
public:
	void run();

private:

	// GLFW window
	GLFWwindow* window;

	// Vulkan Instance
	VkInstance instance;

	// Physical device
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// Logical device
	VkDevice device;

	// Screen surface
	VkSurfaceKHR surface;

	// Graphics queue
	VkQueue graphicsQueue;

	// Presentation queue
	VkQueue presentQueue;

	// Swapchain
	VkSwapchainKHR swapChain;

	// Swapchain images
	std::vector<VkImage> swapChainImages;

	// Swapchain format
	VkFormat swapChainImageFormat;

	// Swapchain extent
	VkExtent2D swapChainExtent;

	// Swapchain image views
	std::vector<VkImageView> swapChainImageViews;

	// Debug report callback
	VkDebugReportCallbackEXT callback;

	void initWindow();

	void initVulkan();

	void createInstance();

	// Check Support for Validation Layers
	bool checkValidationLayerSupport();

	// Gets required extensions
	std::vector<const char*> getRequiredExtensions();

	
	void setupDebugCallback();

	
	// Callback for debbugging through validation layers
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData) {

		std::cerr << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}

		
	// Creates Window using GLFW
	void createSurface();

		// Choose physical device
	void pickPhysicalDevice();

	// Check if a device has the properties and features needed to run this application
	int rateDeviceSuitability(VkPhysicalDevice device);

	// Find queue families
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	// Check device extension support
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// Create logical device
	void createLogicalDevice();

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	void createSwapChain();
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();
	
	void createGraphicsPipeline();

	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	void mainLoop();

	void cleanup();

};