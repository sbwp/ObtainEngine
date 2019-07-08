#ifndef OBTAIN_GRAPHICS_VULKAN_MEMORY_BLOCK_HPP
#define OBTAIN_GRAPHICS_VULKAN_MEMORY_BLOCK_HPP

#include <vector>
#include <vulkan/vulkan.hpp>
#include "memory.hpp"

namespace Obtain::Graphics::Vulkan {
	class MemoryBlock {
	public:

	private:
		vk::UniqueDeviceMemory deviceMemory;
		uint32_t type;
		std::vector<Memory> allocations;
		std::vector<Memory> freedAllocations;
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_MEMORY_BLOCK_HPP
