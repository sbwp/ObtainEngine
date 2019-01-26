#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

#include "instance.hpp"

namespace Obtain::Graphics::Vulkan {
	class Swapchain {
		public:
		Swapchain(Instance *instance);
		~Swapchain();
		
		static void recreateSwapchain(Swapchain *swapchain);
		
		private:
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> swapchainImages;
		vk::Format swapchainImageFormat;
		vk::Extent2D swapchainExtent;
		Instance *instance;
		
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	};
}

#endif // SWAPCHAIN_HPP
