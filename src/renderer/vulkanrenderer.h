#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // vulkan

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
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
#include "uniformbufferobject.h"

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

	// Descriptor set layout
	VkDescriptorSetLayout descriptorSetLayout;

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

	// Textures
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	// Semaphores for synchronizing queue operations
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	// Fences for limiting frames written to at once
	std::vector<VkFence> inFlightFences;

	// TODO: Use an offset to store them in the same VkBuffer to be more cache friendly
	// Vertex buffer
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	// Index buffer
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// Uniform buffers
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	// Descriptor Pool
	VkDescriptorPool descriptorPool;

	// Descriptor Sets
	std::vector<VkDescriptorSet> descriptorSets;

	// (Temporary) hard-coded vertices to draw
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	// (Temporary) hard-coded indices of vertices to draw
	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	// Index of current frame for tracking semaphore to use
	size_t currentFrame = 0;

	// Flag for whether to resize frame buffer
	bool framebufferResized = false;
	
	// Depth
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

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

	void createDescriptorSetLayout();
	
	void createGraphicsPipeline();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	void createFramebuffers();

	void createCommandPool();
	
	void createBuffer(
		VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory
	);
	
	void createDepthResources();
	
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	void createTextureImage();

	void createTextureImageView();
	
	void createTextureSampler();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	void createImage(
		uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory
	);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void createVertexBuffer();
	
	void updateVertices();

	void createIndexBuffer();
	
	void updateIndices();

	void createUniformBuffer();

	void createDescriptorPool();

	void createDescriptorSets();

	// Clamps a float between min and max
	static float fclamp (float value, float min, float max);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createCommandBuffers();

	void createSyncObjects();
	
	void mainLoop();

	void drawFrame();

	void updateUniformBuffer(uint32_t currentImage);

	void cleanup();

};

#endif //VULKAN_RENDERER_H