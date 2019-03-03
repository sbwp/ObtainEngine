#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../renderer.hpp"
#include "swapchain.hpp"

namespace Obtain::Graphics::Vulkan {
	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer(
			std::string gameTitle,
			std::array<uint32_t, 3> gameVersion,
			std::array<uint32_t, 3> engineVersion
		);
		~VulkanRenderer();
		void run();

	private:
		std::string gameTitle;
		std::array<uint32_t, 3> gameVersion;
		std::array<uint32_t, 3> engineVersion;
		std::array<u_int32_t, 2> windowSize;
		
		GLFWwindow* window;
		vk::UniqueInstance instance;
		
		vk::DispatchLoaderDynamic loader;
		vk::DebugUtilsMessengerEXT debugMessenger;
		
		vk::UniqueSurfaceKHR surface;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		
		vk::Queue graphicsQueue;
		vk::Queue presentationQueue;
		
		Swapchain *swapchain;
		
		void initWindow();
	};
}

#endif // VULKAN_RENDERER_HPP