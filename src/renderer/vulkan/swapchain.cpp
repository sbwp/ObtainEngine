#include "swapchain.hpp"
#include "swapchainsupportdetails.hpp"

namespace Obtain::Graphics::Vulkan {
	
	/******************************************
	 ***************** public *****************
	 ******************************************/
	Swapchain::Swapchain(
		vk::UniqueInstance &instance,
		vk::PhysicalDevice physicalDevice,
		vk::UniqueDevice &device,
		vk::SurfaceKHR surface,
		std::array<uint32_t, 2> windowSize,
		QueueFamilyIndices indices
	) 
		:instance(instance), device(device)
	{
		SwapchainSupportDetails swapchainSupport = SwapchainSupportDetails::querySwapchainSupport(
			physicalDevice,
			surface
		);
		
		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
		format = surfaceFormat.format;
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
		extent = chooseSwapExtent(swapchainSupport.capabilities, windowSize);
		
		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
		if (swapchainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapchainSupport.capabilities.maxImageCount
		) {
			imageCount = swapchainSupport.capabilities.maxImageCount;
		}
		
		vk::SharingMode sharingMode = vk::SharingMode::eConcurrent;
		uint32_t queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		uint32_t *pQueueFamilyIndices = queueFamilyIndices;
		
		if (indices.graphicsFamily == indices.presentFamily) {
			sharingMode = vk::SharingMode::eExclusive;
			queueFamilyIndexCount = 0;
			pQueueFamilyIndices = nullptr;
		}
		
		vk::SwapchainCreateInfoKHR createInfo(
			vk::SwapchainCreateFlagsKHR(),
			surface,
			imageCount,
			format,
			surfaceFormat.colorSpace,
			extent,
			1U,
			vk::ImageUsageFlagBits::eColorAttachment,
			sharingMode,
			queueFamilyIndexCount,
			pQueueFamilyIndices,
			swapchainSupport.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode,
			true,
			nullptr
		);
		
		swapchain = device->createSwapchainKHRUnique(createInfo);
		images = device->getSwapchainImagesKHR(*swapchain);
		imageViews.resize(images.size());
		
		for (size_t i = 0; i < images.size(); i++) {
			vk::ImageViewCreateInfo createInfo(
				vk::ImageViewCreateFlags(),
				images[i],
				vk::ImageViewType::e2D,
				format,
				vk::ComponentMapping(
					vk::ComponentSwizzle::eIdentity, // r
					vk::ComponentSwizzle::eIdentity, // g
					vk::ComponentSwizzle::eIdentity, // b
					vk::ComponentSwizzle::eIdentity  // a
				),
				vk::ImageSubresourceRange(
					vk::ImageAspectFlagBits::eColor,
					0U, // base mip level
					1U, // level count
					0U, // base array level
					1U  // layer count
				)
			);
			imageViews[i] = device->createImageView(createInfo);
		}
	}
	
	Swapchain::~Swapchain() {
		for (auto imageView : imageViews) {
			device->destroyImageView(imageView);
		}
	}
	
	/******************************************
	 ***************** private *****************
	 ******************************************/
	vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR>& availableFormats
	) {
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
			return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
		}
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
				availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
			) {
				return availableFormat;
			}
		}
		// Anything else is ok if the preferred isn't there, so just pick the first format
		return availableFormats[0];
	}
	
	vk::PresentModeKHR Swapchain::chooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR> availablePresentModes
	) {
		vk::PresentModeKHR best = vk::PresentModeKHR::eFifo;
		
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				best = availablePresentMode;
			}
		}
		
		return best;
	}
	
	vk::Extent2D Swapchain::chooseSwapExtent(
		const vk::SurfaceCapabilitiesKHR& capabilities,
		std::array<uint32_t, 2> windowSize
	) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			vk::Extent2D actualExtent = {windowSize[0], windowSize[1]};
			
			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width)
			);
			
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height)
			);
			
			return actualExtent;
		}
	}
}