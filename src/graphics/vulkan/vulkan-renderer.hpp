#ifndef OBTAIN_GRAPHICS_VULKAN_VULKAN_RENDERER_HPP
#define OBTAIN_GRAPHICS_VULKAN_VULKAN_RENDERER_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../renderer.hpp"
#include "swapchain.hpp"
#include "object.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "image.hpp"

namespace Obtain::Graphics::Vulkan {
	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer(
			const std::string &gameTitle,
			std::array<uint32_t, 3> gameVersion,
			std::array<uint32_t, 3> engineVersion
		);

		~VulkanRenderer();

		void run();

	private:
		std::unique_ptr<Object> obj;

		QueueFamilyIndices indices;

		Device *device;

		vk::Queue *graphicsQueue;
		vk::Queue *presentationQueue;

		Swapchain *swapchain;
		vk::UniqueCommandPool commandPool;

		vk::UniqueSampler sampler;

		std::unique_ptr<Buffer>  vertexBuffer;
		std::unique_ptr<Buffer>  indexBuffer;

		void drawFrame();

		void updateWindowSize();

		std::unique_ptr<Buffer> createAndLoadBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, void *data);

		void copyBuffer(
			vk::UniqueBuffer &src, vk::DeviceSize srcOffset, vk::UniqueBuffer &dst, vk::DeviceSize dstOffset,
			vk::DeviceSize size
		);
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_VULKAN_RENDERER_HPP
