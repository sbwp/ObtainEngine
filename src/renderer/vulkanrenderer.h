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
#include <ctime>
#include "queuefamilyindices.h"
#include "swapchainsupportdetails.h"
#include "vertex.h"

class VulkanRenderer {
public:
	void run();

private:

	// GLFW window
	GLFWwindow* window;

	// Vulkan Instance
	VkInstance instance;

	// Debug report callback
	VkDebugReportCallbackEXT callback;

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
	
	// Render pass
	VkRenderPass renderPass;

	// Pipeline layout
	VkPipelineLayout pipelineLayout;

	// Pipeline
	VkPipeline graphicsPipeline;

	// Framebuffers
	std::vector<VkFramebuffer> swapChainFramebuffers;

	// Command pool
	VkCommandPool commandPool;

	// Command buffers
	std::vector<VkCommandBuffer> commandBuffers;

	// Semaphores for synchronizing queue operations
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	// Fences for limiting frames written to at once
	std::vector<VkFence> inFlightFences;

	// Vertex buffer
	VkBuffer vertexBuffer;

	// Vertex buffer memory
	VkDeviceMemory vertexBufferMemory;

	// Triangle vertices
	std::vector<Vertex> vertices;

	// Previous time
	clock_t gameTime;

	// Index of current frame for tracking semaphore to use
	size_t currentFrame = 0;

	// Flag for whether to resize frame buffer
	bool framebufferResized = false;

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
		void* userData
	);

	// Callback for handling window resizes
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	
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

	void recreateSwapChain();

	void cleanupSwapChain();
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();

	void createRenderPass();
	
	void createGraphicsPipeline();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	void createFramebuffers();

	void createCommandPool();
	
	void createBuffer(
		VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory
	);

	void createVertexBuffer();
	
	void updateVertices();

	static float fclamp (float value, float min, float max);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createCommandBuffers();

	void createSyncObjects();
	
	void mainLoop();

	void tick(double deltaTime);

	void drawFrame();

	void cleanup();

};
