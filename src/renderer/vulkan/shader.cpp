#include <string>
#include <fstream>
#include <iostream>
#include <iosfwd>

#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	Shader::Shader(vk::UniqueDevice &device, std::string filename, vk::ShaderStageFlagBits pipelineStage)
		: device(device), stage(pipelineStage), code(readFile(filename))
	{
		try {
			module = device->createShaderModule(
				vk::ShaderModuleCreateInfo(
					vk::ShaderModuleCreateFlags(),
					code.size(),
					reinterpret_cast<uint32_t *>(code.data())
				)
			);
		} catch (std::exception const &e) {
			std::cerr << "Failed to create shader module" << std::endl;
			throw e;
		}

		createInfo = vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(),
			stage,
			module,
			"main",
			nullptr
		);
	}

	Shader::~Shader()
	{
		device->destroyShaderModule(
			module,
			nullptr
		);
	}

	std::vector<char> Shader::readFile(const std::string &filename)
	{
		std::ifstream file(
			filename,
			std::ios::ate | std::ios::binary
		);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(
			buffer.data(),
			static_cast<std::streamsize>(fileSize)
		);
		file.close();

		return buffer;
	}
}