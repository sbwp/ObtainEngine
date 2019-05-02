#ifndef OBTAIN_DESCRIPTOR_HPP
#define OBTAIN_DESCRIPTOR_HPP

#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Descriptor {
	public:
		static vk::UniqueDescriptorSetLayout createDescriptorSetLayout(vk::UniqueDevice &device);
	};
}


#endif // OBTAIN_DESCRIPTOR_HPP
