#include "swapchain.hpp"
#include "swapchainsupportdetails.hpp"
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	Swapchain::Swapchain(Instance *instance)
	: instance(instance)
	{
		vk::PhysicalDevice physicalDevice = instance->physicalDevice;
		vk::Device device = *(instance->device);
		vk::SurfaceKHR surface = instance->surface;
		SwapchainSupportDetails swapchainSupport = SwapchainSupportDetails::querySwapchainSupport(physicalDevice, surface);
		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
		if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
			imageCount = swapchainSupport.capabilities.maxImageCount;
		}
		
		QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		bool familiesSame = indices.graphicsFamily == indices.presentFamily;
		
		auto createInfo = new vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			surface, // surface
			imageCount, // minImageCount
			surfaceFormat.format, // imageFormat
			surfaceFormat.colorSpace, // imageColorSpace
			extent, // imageExtent
			1, // imageArrayLayers
			vk::ImageUsageFlagBits::eColorAttachment, // imageUsage
			familiesSame ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent, // imageSharingMode
			familiesSame ? 0 : 2, // queueFamilyIndexCount
			familiesSame ? nullptr : queueFamilyIndices, // pQueueFamilyIndices
			swapchainSupport.capabilities.currentTransform, // preTransform
			vk::CompositeAlphaFlagBitsKHR::eOpaque, // compositeAlpha
			presentMode, // presentMode
			true, // clipped
			nullptr // oldSwapchain
		);
		swapchain = device.createSwapchainKHR(*createInfo, nullptr);

		swapchainImages = device.getSwapchainImagesKHR(swapchain);

		// Store image format and extent in class members
		swapchainImageFormat = surfaceFormat.format;
		swapchainExtent = extent;
	}

	Swapchain::~Swapchain() {
		vkDestroyImageView(device, colorImageView, nullptr);
		vkDestroyImage(device, colorImage, nullptr);
		vkFreeMemory(device, colorImageMemory, nullptr);
		
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);
		
		for (auto framebuffer : swapchainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapchainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}

	vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		
		// If we can choose any one we want, pick the ones we want
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
			return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
		}
		// Otherwise, check if the ones we want are available, and if so, pick them
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinearKHR) {
				return availableFormat;
			}
		}

		// Settle for the first one available
		return availableFormats[0];
	}

	vk::PresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
		
		// By default, settle for FIFO, which is guaranteed to be available
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

		//Check if Mailbox is available
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				bestMode = availablePresentMode;
			}
		}

		return bestMode;
	}

	vk::Extent2D Swapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			std::array<int, 2> size = instance->getWindowSize();

			vk::Extent2D actualExtent = {
				static_cast<uint32_t>(size[0]),
				static_cast<uint32_t>(size[1])
			};

			actualExtent.width = std::max(capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;

		}
	}
	
	void Swapchain::recreateSwapchain(Swapchain *swapchain) {
		// Get this from swapchain before swapchain is destroyed
		Instance *instance = swapchain->instance;
		
		// Wait until the window is visible to bother doing anything.
		instance->waitSizeNonzero();
		instance->device->waitIdle();
		
		delete(swapchain);
		
		swapchain = new Swapchain(instance);
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createColorResources();
		createDepthResources();
		createFramebuffers();
		createCommandBuffers();
	}
}