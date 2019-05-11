#ifndef QUEUE_FAMILY_INDICES_HPP
#define QUEUE_FAMILY_INDICES_HPP

#include <optional>

namespace Obtain::Graphics::Vulkan {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
}
#endif // QUEUE_FAMILY_INDICES_HPP
