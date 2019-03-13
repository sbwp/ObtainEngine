#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

#include "queuefamilyindices.hpp"

namespace Obtain::Graphics::Vulkan {
	class Pipeline {
	public:
		static vk::Pipeline createPipeline(vk::Device device);
	};
}

#endif // PIPELINE_HPP
