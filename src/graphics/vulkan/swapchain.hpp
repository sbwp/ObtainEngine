#ifndef OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_HPP
#define OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

#include "queue-family-indices.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "image.hpp"

namespace Obtain::Graphics::Vulkan {
	class Swapchain {
	public:
		Swapchain(
			Device *device,
			std::array<uint32_t, 2> windowSize,
			QueueFamilyIndices indices,
			vk::UniqueCommandPool &commandPool,
			std::unique_ptr<Buffer> &vertexBuffer,
			std::unique_ptr<Buffer> &indexBuffer,
			std::unique_ptr<Image> &textureImage,
			vk::UniqueSampler &sampler
		);

		~Swapchain();

		bool submitFrame(
			vk::Queue &graphicsQueue,
			vk::Queue &presentationQueue
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
		std::unique_ptr<Image> depthImage;
		std::unique_ptr<Image> colorImage;

		std::vector<vk::UniqueFramebuffer> framebuffers;

		Device *device;

		vk::UniquePipelineLayout pipelineLayout;
		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		std::vector<vk::UniqueDescriptorSet> descriptorSets;
		vk::UniquePipeline pipeline;
		vk::UniqueRenderPass renderPass;

		vk::UniqueCommandPool &commandPool;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
		std::unique_ptr<Buffer> &vertexBuffer;
		std::unique_ptr<Buffer> &indexBuffer;
		std::vector<std::unique_ptr<Buffer>> uniformBuffers;

		static const int MaxFramesInFlight = 2;
		std::array<vk::UniqueSemaphore, MaxFramesInFlight> imageReady;
		std::array<vk::UniqueSemaphore, MaxFramesInFlight> renderFinished;
		std::array<vk::UniqueFence, MaxFramesInFlight> outOfFlight;
		size_t currentFrame = 0;

		std::unique_ptr<Image> &textureImage;
		vk::UniqueSampler &sampler;

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

		void createUniformBuffers();
		void createCommandBuffers();
		void createPipeline();

		void updateUniformBuffer(uint32_t currentImage);
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_HPP
