#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

#endif // QUEUE_FAMILY_INDICES_H
