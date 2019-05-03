#ifndef DEVICE_HPP
#define DEVICE_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "queue-family-indices.hpp"
#include "swapchain-support-details.hpp"

namespace Obtain::Graphics::Vulkan {
	class Buffer; // Forward declaration

	class Device {
	public:
		Device(vk::UniqueInstance &instance, GLFWwindow *window);

		QueueFamilyIndices getQueueFamilies();
		vk::Queue *getGraphicsQueue();
		vk::Queue *getPresentQueue();

		SwapchainSupportDetails querySwapchainSupport();
		SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice physicalDeviceCandidate);
		vk::UniqueSwapchainKHR createSwapchain(vk::SurfaceFormatKHR surfaceFormat,
		                                       const vk::Extent2D &extent,
		                                       vk::PresentModeKHR presentMode);
		std::vector<vk::Image> getSwapchainImages(vk::UniqueSwapchainKHR &swapchain);
		std::vector<vk::ImageView> generateSwapchainImageViews(std::vector<vk::Image> &images, vk::Format format);
		void destroyImageViews(std::vector<vk::ImageView> imageViews);
		uint32_t nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueSemaphore &triggerSemaphore);
		uint32_t nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueFence &triggerFence);
		vk::UniqueImage createImage(const vk::Extent3D &extent, const vk::Format &format,
		                            const vk::ImageTiling &tiling, const vk::ImageUsageFlags &usageFlags);
		vk::MemoryRequirements getImageMemoryRequirements(vk::UniqueImage &image);
		void bindImageMemory(vk::UniqueImage &image, vk::UniqueDeviceMemory &memory, uint32_t offset);

		vk::UniqueDescriptorSetLayout createDescriptorSetLayout();
		vk::UniqueDescriptorPool createDescriptorPool(uint32_t size);
		std::vector<vk::UniqueDescriptorSet> createDescriptorSets(uint32_t size,
		                                                          vk::UniqueDescriptorSetLayout &descriptorSetLayout,
		                                                          vk::UniqueDescriptorPool &descriptorPool,
		                                                          std::vector<std::unique_ptr<Buffer>> &uniformBuffers);

		vk::UniqueSemaphore createSemaphore();
		vk::UniqueFence createFence(bool signaled = false);
		void waitForFence(vk::UniqueFence &fence);
		void resetFence(vk::UniqueFence &fence);

		vk::UniqueShaderModule createShaderModule(size_t size, void *data);
		vk::UniqueRenderPass createRenderPass(const vk::Format &format);
		vk::UniquePipeline createGraphicsPipeline(const vk::Extent2D &extent,
		                                          vk::UniquePipelineLayout &pipelineLayout,
		                                          vk::UniqueRenderPass &renderPass,
		                                          vk::PipelineShaderStageCreateInfo *shaderCreateInfos);
		std::vector<vk::UniqueFramebuffer> createFramebuffers(std::vector<vk::ImageView> imageViews,
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
	private:
		vk::PhysicalDevice physicalDevice;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueDevice device;

		vk::Queue graphicsQueue;
		vk::Queue presentQueue;

		QueueFamilyIndices queueFamilyIndices;

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

	};
}

#endif // DEVICE_HPP
