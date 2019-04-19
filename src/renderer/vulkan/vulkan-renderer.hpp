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
		bool resizeOccurred;

		GLFWwindow *window;
		vk::UniqueInstance instance;
		QueueFamilyIndices indices;

		vk::DispatchLoaderDynamic loader;
		vk::DebugUtilsMessengerEXT debugMessenger;

		vk::UniqueSurfaceKHR surface;
		std::unique_ptr<vk::PhysicalDevice> physicalDevice;
		vk::UniqueDevice device;

		vk::Queue graphicsQueue;
		vk::Queue presentationQueue;

		Swapchain *swapchain;
		vk::UniqueCommandPool commandPool;

		void initWindow();
		void drawFrame();
		void createCommandPool();
		void updateWindowSize();
	};
}

#endif // VULKAN_RENDERER_HPP
