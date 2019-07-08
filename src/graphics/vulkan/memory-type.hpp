#ifndef OBTAIN_GRAPHICS_VULKAN_MEMORY_TYPE_HPP
#define OBTAIN_GRAPHICS_VULKAN_MEMORY_TYPE_HPP

#include <vector>
#include <vulkan/vulkan.hpp>
#include "memory-block.hpp"

namespace Obtain::Graphics::Vulkan {
	class MemoryType {
	public:
		explicit MemoryType(uint32_t type);

		uint32_t getType() const;
	private:
		std::vector<MemoryBlock> blocks;
		const uint32_t type;
	};
}

namespace std {
	template<>
	struct hash<Obtain::Graphics::Vulkan::MemoryType> {
		size_t operator()(Obtain::Graphics::Vulkan::MemoryType const &memoryType) const
		{
			return memoryType.getType();
		}
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_MEMORY_TYPE_HPP
