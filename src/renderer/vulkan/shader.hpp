#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Shader {
		public:
			Shader(vk::Device device, std::string filename, vk::ShaderStageFlagBits pipelineStage);
			~Shader();
			
			inline vk::PipelineShaderStageCreateInfo getCreateInfo() const { return createInfo; }
			inline vk::ShaderModule getModule() const { return module; }
		private:
			std::vector<char> code;
			vk::ShaderStageFlagBits stage;
			vk::ShaderModule module;
			vk::PipelineShaderStageCreateInfo createInfo;
			vk::Device device;
			
			static std::vector<char> readFile(const std::string& filename);
	};
}

#endif // SHADER_HPP