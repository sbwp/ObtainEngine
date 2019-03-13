#include <string>
#include <fstream>
#include <iostream>

#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	Shader::Shader(vk::Device logicalDevice, std::string filename, vk::ShaderStageFlagBits pipelineStage) {
		code = readFile(filename);
		stage = pipelineStage;
		device = logicalDevice;
		
		auto moduleCreateInfo = vk::ShaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),
			code.size(),
			reinterpret_cast<const uint32_t*>(code.data())
		);
		
		try {
			module = device.createShaderModule(moduleCreateInfo);
		} catch(std::exception const &e) {
			std::cerr << "Failed to create shader module" << std::endl;
			throw e;
		}
		
		createInfo = vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(), stage, module, "main", nullptr
		);
	}
	
	Shader::~Shader() {
		device.destroyShaderModule(module, nullptr);
	}
	
	std::vector<char> Shader::readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

				if (!file.is_open()) {
					throw std::runtime_error("failed to open file!");
				}

				size_t fileSize = (size_t) file.tellg();
				std::vector<char> buffer(fileSize);

				file.seekg(0);
				file.read(buffer.data(), fileSize);
				file.close();
				
				return buffer;
	}
}