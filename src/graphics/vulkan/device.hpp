#ifndef OBTAIN_GRAPHICS_VULKAN_DEVICE_HPP
#define OBTAIN_GRAPHICS_VULKAN_DEVICE_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "queue-family-indices.hpp"
#include "swapchain-support-details.hpp"
#include "allocator.hpp"

namespace Obtain::Graphics::Vulkan {
	class Buffer; // Forward declaration

	class Device {
	public:
		Device(const std::string &gameTitle,
		       std::array<uint32_t, 3> gameVersion,
		       std::array<uint32_t, 3> engineVersion);

		~Device();

		QueueFamilyIndices getQueueFamilies();
		vk::Queue *getGraphicsQueue();
		vk::Queue *getPresentQueue();

		SwapchainSupportDetails querySwapchainSupport();
		SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice physicalDeviceCandidate);
		vk::UniqueSwapchainKHR createSwapchain(vk::SurfaceFormatKHR surfaceFormat,
		                                       const vk::Extent2D &extent,
		                                       vk::PresentModeKHR presentMode);
		std::vector<vk::Image> getSwapchainImages(vk::UniqueSwapchainKHR &swapchain);
		std::vector<vk::ImageView> generateSwapchainImageViews(std::vector<vk::Image> &images, const vk::Format &format);
		void destroyImageViews(std::vector<vk::ImageView> imageViews);
		uint32_t nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueSemaphore &triggerSemaphore);
		uint32_t nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueFence &triggerFence);
		vk::UniqueImage createImage(const vk::Extent3D &extent, const vk::Format &format, uint32_t mipLevels,
		                            const vk::ImageTiling &tiling, const vk::ImageUsageFlags &usageFlags,
		                            vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1);
		vk::UniqueImageView createImageView(vk::UniqueImage &image, const vk::Format &format, uint32_t mipLevels,
		                                    const vk::ImageAspectFlags &aspectMask);
		vk::UniqueSampler createSampler(float mipLevels);
		vk::FormatProperties getFormatProperties(const vk::Format &format);

		vk::MemoryRequirements getImageMemoryRequirements(vk::UniqueImage &image);
		void bindImageMemory(vk::UniqueImage &image, vk::UniqueDeviceMemory &memory, uint32_t offset);

		vk::UniqueDescriptorSetLayout createDescriptorSetLayout();
		vk::UniqueDescriptorPool createDescriptorPool(uint32_t size);
		std::vector<vk::UniqueDescriptorSet> createDescriptorSets(uint32_t size,
		                                                          vk::UniqueDescriptorSetLayout &descriptorSetLayout,
		                                                          vk::UniqueDescriptorPool &descriptorPool,
		                                                          vk::UniqueSampler &sampler,
		                                                          vk::UniqueImageView &imageView,
		                                                          std::vector<std::unique_ptr<Buffer>> &uniformBuffers);

		vk::UniqueSemaphore createSemaphore();
		vk::UniqueFence createFence(bool signaled = false);
		void waitForFence(vk::UniqueFence &fence);
		void resetFence(vk::UniqueFence &fence);

		vk::UniqueShaderModule createShaderModule(size_t size, void *data);
		vk::UniqueRenderPass createRenderPass(const vk::Format &colorFormat, const vk::Format &depthFormat);
		vk::UniquePipeline createGraphicsPipeline(const vk::Extent2D &extent,
		                                          vk::UniquePipelineLayout &pipelineLayout,
		                                          vk::UniqueRenderPass &renderPass,
		                                          vk::PipelineShaderStageCreateInfo *shaderCreateInfos);
		std::vector<vk::UniqueFramebuffer> createFramebuffers(std::vector<vk::ImageView> imageViews,
		                                                      vk::UniqueImageView &colorImageView,
		                                                      vk::UniqueImageView &depthImageView,
		                                                      vk::UniqueRenderPass &renderPass,
		                                                      const vk::Extent2D &extent);
		std::vector<vk::UniqueCommandBuffer> allocateCommandBuffers(vk::UniqueCommandPool &commandPool,
		                                                            vk::CommandBufferLevel level,
		                                                            uint32_t count);
		vk::UniqueCommandPool createCommandPool();

		vk::UniquePipelineLayout createPipelineLayout(vk::UniqueDescriptorSetLayout &descriptorSetLayout);

		void waitIdle();

		vk::UniqueBuffer createBuffer(vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags);
		uint32_t findMemoryType(uint32_t typeFilter, const vk::MemoryPropertyFlags &properties);
		vk::UniqueDeviceMemory allocateMemory(vk::DeviceSize size, uint32_t memoryType);
		vk::MemoryRequirements getBufferMemoryRequirements(vk::UniqueBuffer &buffer);
		void bindBufferMemory(vk::UniqueBuffer &buffer, vk::UniqueDeviceMemory &memory, uint32_t offset);

		void *mapMemory(vk::UniqueDeviceMemory &memory, uint32_t offset, vk::DeviceSize size);
		void unmapMemory(vk::UniqueDeviceMemory &memory);
		void setMemory(vk::UniqueDeviceMemory &memory, uint32_t offset, vk::DeviceSize size, void *src);

		bool windowOpen();
		std::array<uint32_t, 2> updateWindowSizeOnceVisible();
		std::array<uint32_t, 2> getWindowSize();
		void setWindowSize(uint32_t width, uint32_t height);
		void resetResizeFlag();
		bool getResizeFlag();

		bool hasOptimalTilingFeature(const vk::Format &format, const vk::FormatFeatureFlags &feature);
		vk::SampleCountFlagBits getSampleCount();
	private:
		vk::UniqueInstance instance;
		GLFWwindow *window;
		bool resizeOccurred;

		std::array<u_int32_t, 2> windowSize;

		vk::PhysicalDevice physicalDevice;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueDevice device;
		std::unique_ptr<Allocator> allocator;

		std::string gameTitle;
		std::array<uint32_t, 3> gameVersion;
		std::array<uint32_t, 3> engineVersion;

		vk::DispatchLoaderDynamic loader;
		vk::DebugUtilsMessengerEXT debugMessenger;

		vk::Queue graphicsQueue;
		vk::Queue presentQueue;

		QueueFamilyIndices queueFamilyIndices;
		vk::SampleCountFlagBits sampleCount;


		void findSampleCount();
		QueueFamilyIndices findQueueFamilies();
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &physicalDeviceCandidate);

		vk::PhysicalDevice selectPhysicalDevice(
			vk::UniqueInstance &instance
		);

		vk::UniqueDevice createLogicalDevice(
			vk::UniqueInstance &instance
		);

		static vk::UniqueSurfaceKHR createSurface(const vk::Instance &instance, GLFWwindow *window);

		static const std::vector<const char *> deviceExtensions;

		uint32_t ratePhysicalDeviceSuitability(const vk::PhysicalDevice &physicalDeviceCandidate);

		static bool checkDeviceExtensionSupport(const vk::PhysicalDevice &physicalDeviceCandidate);

		static bool checkForSupportedExtensions(std::vector<const char *> requiredExtensions);

		static std::vector<const char *> getRequiredExtensions(bool useValidationLayers);

		void createInstance();

		void createWindow();

		static vk::SampleCountFlags min(const vk::SampleCountFlags &a, const vk::SampleCountFlags &b);

	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_DEVICE_HPP
