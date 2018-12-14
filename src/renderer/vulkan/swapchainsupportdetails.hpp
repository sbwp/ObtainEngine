#ifndef SWAP_CHAIN_SUPPORT_DETAILS_HPP
#define SWAP_CHAIN_SUPPORT_DETAILS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
}
#endif // SWAP_CHAIN_SUPPORT_DETAILS_HPP