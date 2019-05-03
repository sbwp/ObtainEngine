#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Shader {
	public:
		Shader(std::unique_ptr<Device> &device, const std::string &filename, const vk::ShaderStageFlagBits &pipelineStage);

		const vk::PipelineShaderStageCreateInfo getCreateInfo();

		const vk::UniqueShaderModule &getModule();

	private:
		std::vector<char> code;
		vk::ShaderStageFlagBits stage;
		vk::UniqueShaderModule module;
		vk::PipelineShaderStageCreateInfo createInfo;
		std::unique_ptr<Device> &device;

		static std::vector<char> readFile(const std::string &filename);
	};
}

#endif // SHADER_HPP