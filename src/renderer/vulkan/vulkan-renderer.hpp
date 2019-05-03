#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../renderer.hpp"
#include "swapchain.hpp"
#include "object.hpp"
#include "buffer.hpp"
#include "device.hpp"

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
		Object obj = Object(); // TODO: Replace with actual vertices

		GLFWwindow *window;
		vk::UniqueInstance instance;
		QueueFamilyIndices indices;

		vk::DispatchLoaderDynamic loader;
		vk::DebugUtilsMessengerEXT debugMessenger;

		std::unique_ptr<Device> device;

		vk::Queue *graphicsQueue;
		vk::Queue *presentationQueue;

		Swapchain *swapchain;
		vk::UniqueCommandPool commandPool;

		std::unique_ptr<Buffer>  vertexBuffer;
		std::unique_ptr<Buffer>  indexBuffer;

		void initWindow();

		void drawFrame();

		void updateWindowSize();

		std::unique_ptr<Buffer> createAndLoadBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, void *data);

		void copyBuffer(
			vk::UniqueBuffer &src, vk::DeviceSize srcOffset, vk::UniqueBuffer &dst, vk::DeviceSize dstOffset,
			vk::DeviceSize size
		);
	};
}

#endif // VULKAN_RENDERER_HPP
