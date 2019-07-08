//
// Created by sabrina on 4/22/19.
//

#ifndef OBTAIN_GRAPHICS_VULKAN_BUFFER_HPP
#define OBTAIN_GRAPHICS_VULKAN_BUFFER_HPP

#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Buffer {
	public:
		Buffer(Device *device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
		       const vk::MemoryPropertyFlags &propertyFlags);

		static std::unique_ptr<Buffer> unique(Device *device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
		const vk::MemoryPropertyFlags &propertyFlags);

		void load(vk::DeviceSize internalOffset, void *source, size_t size);

		vk::UniqueBuffer &getBuffer();

		vk::UniqueDeviceMemory &getDeviceMemory();

		vk::DeviceSize getSize();

		vk::DeviceSize getOffset();

		void copyToImage(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue, vk::UniqueImage &image,
		                 const vk::BufferImageCopy &region, const vk::ImageSubresourceLayers &subresource);

		void copyToBuffer(vk::UniqueCommandPool &pool, vk::Queue *queue, std::unique_ptr<Buffer> &dst);

	private:
		vk::DeviceSize size;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;
		vk::DeviceSize offset;

		Device *device;
	};
}


#endif // OBTAIN_GRAPHICS_VULKAN_BUFFER_HPP
