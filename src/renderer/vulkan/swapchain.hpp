#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

#include "queue-family-indices.hpp"
#include "buffer.hpp"

namespace Obtain::Graphics::Vulkan {
	class Swapchain {
	public:
		Swapchain(
			vk::UniqueInstance &instance,
			std::unique_ptr<vk::PhysicalDevice> &physicalDevice,
			vk::UniqueDevice &device,
			vk::UniqueSurfaceKHR &surface,
			std::array<uint32_t, 2> windowSize,
			QueueFamilyIndices indices,
			vk::UniqueCommandPool &commandPool,
			std::unique_ptr<Buffer> &vertexBuffer,
			std::unique_ptr<Buffer> &indexBuffer
		);

		~Swapchain();

		bool submitFrame(
			vk::Queue graphicsQueue,
			vk::Queue presentationQueue
		);

		inline vk::UniqueSwapchainKHR &getSwapchain()
		{
			return swapchain;
		}

		inline vk::UniqueCommandBuffer &getCommandBuffer(uint32_t i)
		{
			return commandBuffers[i];
		}

	private:
		vk::UniqueSwapchainKHR swapchain;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		vk::Format format;
		vk::Extent2D extent;

		std::vector<vk::UniqueFramebuffer> framebuffers;

		vk::UniqueInstance &instance;
		vk::UniqueDevice &device;
		std::unique_ptr<vk::PhysicalDevice> &physicalDevice;
		vk::UniqueSurfaceKHR &surface;

		vk::UniquePipelineLayout layout;
		vk::UniquePipeline pipeline;
		vk::UniqueRenderPass renderPass;

		vk::UniqueCommandPool &commandPool;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
		std::unique_ptr<Buffer> &vertexBuffer;
		std::unique_ptr<Buffer> &indexBuffer;

		static const int MaxFramesInFlight = 2;
		std::array<vk::UniqueSemaphore, MaxFramesInFlight> imageReady;
		std::array<vk::UniqueSemaphore, MaxFramesInFlight> renderFinished;
		std::array<vk::UniqueFence, MaxFramesInFlight> outOfFlight;
		size_t currentFrame = 0;

		static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<vk::SurfaceFormatKHR> &availableFormats
		);

		static vk::PresentModeKHR chooseSwapPresentMode(
			const std::vector<vk::PresentModeKHR> &availablePresentModes
		);

		static vk::Extent2D chooseSwapExtent(
			const vk::SurfaceCapabilitiesKHR &capabilities,
			std::array<uint32_t, 2> windowSize
		);

		void createPipeline();

		void createRenderPass();

		void createFramebuffers();

		void createCommandBuffers();
	};
}

#endif // SWAPCHAIN_HPP
