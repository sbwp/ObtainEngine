//
// Created by austin on 4/22/19.
//

#include "buffer.hpp"

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	Buffer::Buffer(
		vk::UniqueDevice &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice, vk::DeviceSize size,
		vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags
	)
		: device(device), physicalDevice(physicalDevice)
	{
		buffer = device->createBufferUnique(
			vk::BufferCreateInfo(
				vk::BufferCreateFlags(),
				size,
				usageFlags,
				vk::SharingMode::eExclusive
			)
		);

		auto memoryRequirements = device->getBufferMemoryRequirements(*buffer);

		memory = device->allocateMemoryUnique(
			vk::MemoryAllocateInfo(
				memoryRequirements.size,
				Device::findMemoryType(
					*physicalDevice,
					memoryRequirements.memoryTypeBits,
					propertyFlags
				)
			)
		);

		device->bindBufferMemory(
			*buffer,
			*memory,
			0u
		);
	}

	void Buffer::load(vk::DeviceSize offset, void *source, vk::DeviceSize size)
	{
		void *data = device->mapMemory(
			*memory,
			offset,
			size
		);

		memcpy(
			data,
			source,
			static_cast<size_t>(size));

		device->unmapMemory(*memory);
	}
}