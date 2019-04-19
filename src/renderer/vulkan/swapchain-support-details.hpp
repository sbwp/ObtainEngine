#ifndef SWAPCHAIN_SUPPORT_DETAILS_HPP
#define SWAPCHAIN_SUPPORT_DETAILS_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;

		static SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device, vk::UniqueSurfaceKHR &surface) {
			SwapchainSupportDetails details;

			details.capabilities = device.getSurfaceCapabilitiesKHR(*surface);

			// Check size of formats
			details.formats = device.getSurfaceFormatsKHR(*surface);

			details.presentModes = device.getSurfacePresentModesKHR(*surface);

			return details;
		}
	};
}
#endif // SWAPCHAIN_SUPPORT_DETAILS_HPP
