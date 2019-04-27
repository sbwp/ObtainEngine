//
// Created by sabrina on 4/22/19.
//

#ifndef OBTAIN_BUFFER_HPP
#define OBTAIN_BUFFER_HPP

#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Buffer {
	public:
		Buffer(
			vk::UniqueDevice &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice, vk::DeviceSize size,
			vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags
		);

		void load(vk::DeviceSize offset, void *source, size_t size);

		vk::UniqueBuffer &getBuffer();

		vk::UniqueDeviceMemory &getDeviceMemory();

		vk::DeviceSize getSize();

	private:
		vk::DeviceSize size;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory memory;

		std::unique_ptr<vk::PhysicalDevice> &physicalDevice;
		vk::UniqueDevice &device;
	};
}


#endif // OBTAIN_BUFFER_HPP
