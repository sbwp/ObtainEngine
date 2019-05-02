#include "descriptor.hpp"

namespace Obtain::Graphics::Vulkan {
	vk::UniqueDescriptorSetLayout Descriptor::createDescriptorSetLayout(vk::UniqueDevice &device) {
		vk::DescriptorSetLayoutBinding layoutInfo(
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eVertex,
			nullptr
		);

		return device->createDescriptorSetLayoutUnique(
			vk::DescriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				1,
				&layoutInfo
			)
		);
	}
}