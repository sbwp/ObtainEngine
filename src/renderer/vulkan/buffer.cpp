//
// Created by austin on 4/22/19.
//

#include "buffer.hpp"

namespace Obtain::Graphics::Vulkan {
	Buffer::Buffer(std::unique_ptr<Device> &device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
	               const vk::MemoryPropertyFlags &propertyFlags)
		: device(device), size(size)
	{
		buffer = device->createBuffer(size, usageFlags);

		auto memoryRequirements = device->getBufferMemoryRequirements(buffer);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits,
		                                        propertyFlags);

		memory = device->allocateMemory(memoryRequirements.size,
		                               memoryType);

		offset = 0u;

		device->bindBufferMemory(buffer, memory, offset);
	}

	void Buffer::load(vk::DeviceSize internalOffset, void *source, vk::DeviceSize size)
	{
		device->setMemory(memory, offset + internalOffset, size, source);
	}

	vk::UniqueBuffer &Buffer::getBuffer() {
		return buffer;
	}

	vk::UniqueDeviceMemory &Buffer::getDeviceMemory() {
		return memory;
	}

	vk::DeviceSize Buffer::getSize() {
		return size;
	}
}