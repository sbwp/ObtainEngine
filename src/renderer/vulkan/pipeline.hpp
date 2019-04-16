#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

#include "queue-family-indices.hpp"

namespace Obtain::Graphics::Vulkan {
	class Pipeline {
	public:
		static vk::Pipeline createPipeline(vk::Device device, vk::Extent2D extent);
	};
}

#endif // PIPELINE_HPP
