#ifndef SWAP_CHAIN_SUPPORT_DETAILS_H
#define SWAP_CHAIN_SUPPORT_DETAILS_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // glfw vulkan
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif // SWAP_CHAIN_SUPPORT_DETAILS_H