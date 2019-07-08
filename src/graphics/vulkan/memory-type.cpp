#include "memory-type.hpp"

namespace Obtain::Graphics::Vulkan {
	MemoryType::MemoryType(uint32_t type)
		: type(type)
	{}

	uint32_t MemoryType::getType() const
	{
		return type;
	}
}