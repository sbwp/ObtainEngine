#include "pipeline.hpp"

#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	vk::Pipeline Pipeline::createPipeline(vk::Device device) {
		Shader *vertShader = new Shader(device, "basic.vert", vk::ShaderStageFlagBits::eVertex);
		Shader *fragShader = new Shader(device, "basic.frag", vk::ShaderStageFlagBits::eFragment);
		
		auto vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			0,
			nullptr,
			0,
			nullptr
		);
		
		delete(vertShader);
		delete(fragShader);
	}
}