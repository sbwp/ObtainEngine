#include "pipeline.hpp"

#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	vk::Pipeline Pipeline::createPipeline(vk::Device device) {
		Shader *vertShader = new Shader(device, "basic.vert", vk::ShaderStageFlagBits::eVertex);
		Shader *fragShader = new Shader(device, "basic.frag", vk::ShaderStageFlagBits::eFragment);
		
		// TODO: Create Pipeline
		
		delete(vertShader);
		delete(fragShader);
	}
}