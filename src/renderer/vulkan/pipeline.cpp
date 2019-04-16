#include "pipeline.hpp"

#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	vk::Pipeline Pipeline::createPipeline(vk::Device device, vk::Extent2D extent) {
		Shader *vertShader = new Shader(
			device,
			"basic.vert",
			vk::ShaderStageFlagBits::eVertex
		);
		Shader *fragShader = new Shader(
			device,
			"basic.frag",
			vk::ShaderStageFlagBits::eFragment
		);

		auto vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			0,
			nullptr,
			0,
			nullptr
		);

		auto inputAssemblyCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList,
			false
		);

		auto viewport = vk::Viewport(
			0.0f,
			0.0f,
			(float) extent.width,
			(float) extent.height,
			0.0f,
			0.0f
		);

		auto scissor = vk::Rect2D(
			vk::Offset2D(
				0,
				0
			),
			extent
		);

		auto viewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
			vk::PipelineViewportStateCreateFlags(),
			1,
			&viewport,
			1,
			&scissor
		);

		auto rasterizerCreateInfo = vk::PipelineRasterizationStateCreateInfo(
			vk::PipelineRasterizationStateCreateFlags(),
			false,
			false,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eClockwise,
			false,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		);

		auto multisamplingCreateInfo = vk::PipelineMultisampleStateCreateInfo(); // temporarily disabled

		auto colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState(
			false,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		);

		auto colorBlendingCreateInfo = vk::PipelineColorBlendStateCreateInfo(
			vk::PipelineColorBlendStateCreateFlags(),
			false,
			vk::LogicOp::eCopy,
			1,
			&colorBlendAttachmentState
		);

		std::vector<vk::DynamicState> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};

		auto dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo(
			vk::PipelineDynamicStateCreateFlags(),
			static_cast<uint32_t>(dynamicStates.size()),
			dynamicStates.data()
		);

		auto layoutCreateInfo = vk::PipelineLayoutCreateInfo(
			vk::PipelineLayoutCreateFlags(),
			0,
			nullptr,
			0,
			nullptr
		);

		vk::UniquePipelineLayout layout = device.createPipelineLayoutUnique(layoutCreateInfo);

		delete (vertShader);
		delete (fragShader);
	}

}