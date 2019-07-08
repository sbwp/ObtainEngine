#ifndef OBTAIN_GRAPHICS_VULKAN_MEMORY_HPP
#define OBTAIN_GRAPHICS_VULKAN_MEMORY_HPP

#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	struct Memory {
		vk::UniqueDeviceMemory &memory;
		vk::DeviceSize offset = 0U;
		vk::DeviceSize size = 0U;
		uint32_t type = 0U;
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_MEMORY_HPP
