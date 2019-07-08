#ifndef OBTAIN_GRAPHICS_VULKAN_ALLOCATOR_HPP
#define OBTAIN_GRAPHICS_VULKAN_ALLOCATOR_HPP

#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "memory.hpp"
#include "memory-type.hpp"

namespace Obtain::Graphics::Vulkan { class Device; }
namespace vk { typedef DeviceSize uint64_t; }

namespace Obtain::Graphics::Vulkan {
	class Allocator {
	public:
		Allocator(Device *device);
		Memory allocate(vk::DeviceSize size, vk::DeviceSize offset, uint32_t type);
	private:
		Device *device;
		std::unordered_map<MemoryType, uint32_t> allocations;

		static const vk::DeviceSize allocationSize = 4096;
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_ALLOCATOR_HPP
