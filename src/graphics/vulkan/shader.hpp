#ifndef OBTAIN_GRAPHICS_VULKAN_SHADER_HPP
#define OBTAIN_GRAPHICS_VULKAN_SHADER_HPP

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Shader {
	public:
		Shader(Device *device, const std::string &filename, const vk::ShaderStageFlagBits &pipelineStage);

		const vk::PipelineShaderStageCreateInfo getCreateInfo();

		const vk::UniqueShaderModule &getModule();

	private:
		std::vector<char> code;
		vk::ShaderStageFlagBits stage;
		vk::UniqueShaderModule module;
		vk::PipelineShaderStageCreateInfo createInfo;
		Device *device;

		static std::vector<char> readFile(const std::string &filename);
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_SHADER_HPP