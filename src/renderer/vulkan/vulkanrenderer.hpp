#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <vulkan/vulkan.hpp>

#include "queuefamilyindices.hpp"
#include "swapchainsupportdetails.hpp"
#include "renderer.hpp"
#include "vertex.hpp"

namespace Obtain::Graphics::Vulkan {
	class VulkanRenderer : public Renderer {
	public:
		void run();

	private:

		// GLFW window
		GLFWwindow* window;

		// Vulkan Instance
		vk::Instance instance;

		// Debug report callback
		vk::DebugReportCallbackEXT callback;

		// Physical device
		vk::PhysicalDevice physicalDevice = nullptr;

		// Logical device
		vk::Device device;

		// Screen surface
		vk::SurfaceKHR surface;

		// Graphics queue
		vk::Queue graphicsQueue;

		// Presentation queue
		vk::Queue presentQueue;

		// Swapchain
		vk::SwapchainKHR swapChain;

		// Swapchain images
		std::vector<vk::Image> swapChainImages;

		// Swapchain format
		vk::Format swapChainImageFormat;

		// Swapchain extent
		vk::Extent2D swapChainExtent;

		// Swapchain image views
		std::vector<vk::ImageView> swapChainImageViews;
		
		// Render pass
		vk::RenderPass renderPass;

		// Descriptor set layout
		vk::DescriptorSetLayout descriptorSetLayout;

		// Pipeline layout
		vk::PipelineLayout pipelineLayout;

		// Pipeline
		vk::Pipeline graphicsPipeline;

		// Framebuffers
		std::vector<vk::Framebuffer> swapChainFramebuffers;

		// Command pool
		vk::CommandPool commandPool;

		// Command buffers
		std::vector<vk::CommandBuffer> commandBuffers;

		// Textures
		uint32_t mipLevels;
		vk::Image textureImage;
		vk::DeviceMemory textureImageMemory;
		vk::ImageView textureImageView;
		vk::Sampler textureSampler;

		// Semaphores for synchronizing queue operations
		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;

		// Fences for limiting frames written to at once
		std::vector<vk::Fence> inFlightFences;

		// TODO: Use an offset to store them in the same vk::Buffer to be more cache friendly
		// Vertex buffer
		vk::Buffer vertexBuffer;
		vk::DeviceMemory vertexBufferMemory;

		// Index buffer
		vk::Buffer indexBuffer;
		vk::DeviceMemory indexBufferMemory;

		// Uniform buffers
		std::vector<vk::Buffer> uniformBuffers;
		std::vector<vk::DeviceMemory> uniformBuffersMemory;

		// Descriptor Pool
		vk::DescriptorPool descriptorPool;

		// Descriptor Sets
		std::vector<vk::DescriptorSet> descriptorSets;

		// Vertices to draw
		std::vector<Vertex> vertices;

		// Indices of vertices to draw
		std::vector<uint32_t> indices;

		// Index of current frame for tracking semaphore to use
		size_t currentFrame = 0;

		// Flag for whether to resize frame buffer
		bool framebufferResized = false;
		
		// Depth
		vk::Image depthImage;
		vk::DeviceMemory depthImageMemory;
		vk::ImageView depthImageView;
		
		// Max supported samples for antialiasing
		vk::SampleCountFlagBits msaaSamples = vk::SampleCountFlagBits::e1;
		
		// Render target for antialiasing
		vk::Image colorImage;
		vk::DeviceMemory colorImageMemory;
		vk::ImageView colorImageView;

		void initWindow();

		void initVulkan();

		void createInstance();

		// Check Support for Validation Layers
		bool checkValidationLayerSupport();

		// Gets required extensions
		std::vector<const char*> getRequiredExtensions();

		void setupDebugCallback();
		
		// Callback for debbugging through validation layers
		static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
			vk::DebugReportFlagsEXT flags,
			vk::DebugReportObjectTypeEXT objType,
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
		
		// Check maximum MSAA samples
		vk::SampleCountFlagBits getMaxUsableSampleCount();

		// Check if a device has the properties and features needed to run this application
		int rateDeviceSuitability(vk::PhysicalDevice device);

		// Find queue families
		QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

		// Check device extension support
		bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

		// Create logical device
		void createLogicalDevice();

		SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

		void createSwapChain();

		void recreateSwapChain();

		void cleanupSwapChain();
		
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);

		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

		void createImageViews();

		void createRenderPass();

		void createDescriptorSetLayout();
		
		void createGraphicsPipeline();

		vk::ShaderModule createShaderModule(const std::vector<char>& code);

		void createFramebuffers();

		void createCommandPool();
		
		void createBuffer(
			vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
			vk::Buffer& buffer, vk::DeviceMemory& bufferMemory
		);
		
		void createColorResources();
		
		void createDepthResources();
		
		vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

		void createTextureImage();
		
		void generateMipmaps(vk::Image image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		void createTextureImageView();
		
		void createTextureSampler();

		vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipLvls);

		void createImage(
			uint32_t width, uint32_t height, uint32_t mipLvls, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
			vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory
		);

		void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mipLvls);

		void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

		vk::CommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
		
		void loadModel();

		void createVertexBuffer();
		
		void updateVertices();

		void createIndexBuffer();
		
		void updateIndices();

		void createUniformBuffer();

		void createDescriptorPool();

		void createDescriptorSets();

		// Clamps a float between min and max
		static float fclamp (float value, float min, float max);

		void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

		void createCommandBuffers();

		void createSyncObjects();
		
		void mainLoop();

		void drawFrame();

		void updateUniformBuffer(uint32_t currentImage);

		void cleanup();

	};
}

#endif // VULKAN_RENDERER_HPP