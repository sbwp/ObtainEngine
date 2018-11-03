#ifndef SWAP_CHAIN_SUPPORT_DETAILS_HPP
#define SWAP_CHAIN_SUPPORT_DETAILS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif // SWAP_CHAIN_SUPPORT_DETAILS_HPP