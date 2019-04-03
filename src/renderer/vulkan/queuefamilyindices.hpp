#ifndef QUEUE_FAMILY_INDICES_HPP
#define QUEUE_FAMILY_INDICES_HPP

#include <optional>

namespace Obtain::Graphics::Vulkan {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}

		static QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
			QueueFamilyIndices indices;

			std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

			int i = 0;
			for (const auto &queueFamily : queueFamilies) {

				vk::Bool32 presentSupport = false;
				device.getSurfaceSupportKHR(
						i,
						surface,
						&presentSupport
				);

				if (queueFamily.queueCount > 0 && presentSupport) {
					indices.presentFamily = i;
				}

				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
					indices.graphicsFamily = i;
				}

				if (indices.isComplete()) {
					break;
				}

				i++;
			}

			return indices;
		}
	};
}
#endif // QUEUE_FAMILY_INDICES_HPP
