#ifndef OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_SUPPORT_DETAILS_HPP
#define OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_SUPPORT_DETAILS_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
}
#endif // OBTAIN_GRAPHICS_VULKAN_SWAPCHAIN_SUPPORT_DETAILS_HPP
