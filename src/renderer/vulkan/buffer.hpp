//
// Created by austin on 4/22/19.
//

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Buffer {
	public:
		Buffer(std::unique_ptr<Device> &device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
		       const vk::MemoryPropertyFlags &propertyFlags);

		void load(vk::DeviceSize internalOffset, void *source, size_t size);

		vk::UniqueBuffer &getBuffer();

		vk::UniqueDeviceMemory &getDeviceMemory();

		vk::DeviceSize getSize();

	private:
		vk::DeviceSize size;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;
		vk::DeviceSize offset;

		std::unique_ptr<Device> &device;
	};
}


#endif // BUFFER_HPP
