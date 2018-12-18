#include "graphicspipeline.hpp"

#include "vertex.hpp"
#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	GraphicsPipeline::GraphicsPipeline() {
		Shader *vertexShader = new Shader(device, "shaders/vert.spv", vk::ShaderStageFlagBits::eVertex);
		Shader *fragShader = new Shader(device, "shaders/frag.spv", vk::ShaderStageFlagBits::eFragment);

		vk::PipelineShaderStageCreateInfo shaderStages[] = {
			vertexShader->getCreateInfo(),
			fragShader->getCreateInfo()
		};

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		auto vertexInputInfo = new vk::PipelineVertexInputStateCreateInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			1, // vertexBindingDescriptionCount
			&bindingDescription, // pVertexBindingDescriptions
			static_cast<uint32_t>(attributeDescriptions.size()), // vertexAttributeDescriptions
			attributeDescriptions.data() // pVertexAttributeDescriptions
		);
		
		auto inputAssembly = new vk::PipelineInputAssemblyStateCreateInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList, // topology
			false // primitiveRestartEnable
		);

		auto viewport = new vk::Viewport(
			0.0f, // x
			0.0f, // y
			(float) swapchainExtent.width, // width
			(float) swapchainExtent.height, // height
			0.0f, // minDepth
			1.0f // maxDepth
		);
		
		auto scissor = new vk::Rect2D(
			{0,0}, // offset
			swapchainExtent //extent
		);
		
		auto viewportState = new vk::PipelineViewportStateCreateInfo(
			vk::PipelineViewportStateCreateFlags(),
			1, // viewportCount
			viewport, // pViewports
			1, // scissorCount
			scissor // pScissors
		);
		
		auto rasterizer = new vk::PipelineRasterizationStateCreateInfo(
			vk::PipelineRasterizationStateCreateFlags(),
			false, // depthClampEnable
			false, // rasterizerDiscardEnable
			vk::PolygonMode::eFill, // polygonMode
			vk::CullModeFlagBits::eBack, // cullMode
			vk::FrontFace::eCounterClockwise, // frontFace
			false, // depthBiasEnable
			0.0f, // depthBiasConstantFactor
			0.0f, // depthBiasClamp
			0.0f, // depthBiasSlopeFactor
			1.0f // lineWidth
		);
		
		auto multisampling = new vk::PipelineMultisampleStateCreateInfo(
			vk::PipelineMultisampleStateCreateFlags(),
			msaaSamples, //rasterizationSamples
			true, // sampleShadingEnable
			0.2f, // minSampleShading
			nullptr, // pSampleMask
			false, // alphaToCoverageEnable
			false // alphaToOneEnable
		);

		// Depth and stencil testing will go here when we add depth buffering

		auto colorBlendAttachment = new vk::PipelineColorBlendAttachmentState(
			false, // blendEnable
			vk::BlendFactor::eOne, // srcColorBlendFactor
			vk::BlendFactor::eZero, // dstColorBlendFactor
			vk::BlendOp::eAdd, // colorBlendOp
			vk::BlendFactor::eOne, // srcAlphaBlendFactor
			vk::BlendFactor::eZero, // dstAlphaBlendFactor
			vk::BlendOp::eAdd, // alphaBlendOp
			vk::ColorComponentFlagBits::eR | // colorWriteMask
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA
		);
		
		auto colorBlending = new vk::PipelineColorBlendStateCreateInfo(
			vk::PipelineColorBlendStateCreateFlags(),
			false, // logicOpEnable
			vk::LogicOp::eCopy, // logicOp
			1, // attachmentCount
			colorBlendAttachment, // pAttachments
			{0.0f, 0.0f, 0.0f, 0.0f} // blendConstants
		);
		
		auto pipelineLayoutInfo = new vk::PipelineLayoutCreateInfo(
			vk::PipelineLayoutCreateFlags(),
			1, // setLayoutCount
			&descriptorSetLayout, // pSetLayouts
			0, // pushConstantRangeCount
			nullptr // pPushConstantRanges
		);
		pipelineLayout = device.createPipelineLayout(*pipelineLayoutInfo);
		
		auto depthStencil = new vk::PipelineDepthStencilStateCreateInfo(
			vk::PipelineDepthStencilStateCreateFlags(),
			true, // depthTestEnable
			true, // depthWriteEnable
			vk::CompareOp::eLess, // depthCompareOp
			false, // depthBoundsTestEnable
			false, // stencilTestEnable
			vk::StencilOpState(), // front
			vk::StencilOpState(), // black
			0.0f, // minDepthBounds
			0.0f // maxDepthBounds
		);
		
		auto pipelineInfo = new vk::GraphicsPipelineCreateInfo(
			vk::PipelineCreateFlags(),
			2, // stageCount
			shaderStages, // pStages
			vertexInputInfo, // pVertexInputState
			inputAssembly, // pInputAssemblyState
			nullptr, // pTessellationState
			viewportState, // pViewportState
			rasterizer, // pRasterizationState
			multisampling, // pMultisampleState
			depthStencil, // pDepthStencilState
			colorBlending, // pColorBlendState
			nullptr, // pDynamicState
			pipelineLayout, // layout
			renderPass, // renderPass
			0, // subpass
			vk::Pipeline(), // basePipelineHandle
			-1 // basePipelineIndex
		);
		
		device.createGraphicsPipelines(
			device.createPipelineCache(vk::PipelineCacheCreateInfo()), // pipelineCache
			1, // createInfoCount
			pipelineInfo, // pCreateInfos
			nullptr, // pAllocator
			&graphicsPipeline // pPipelines
		);
		
		delete(vertexShader);
		delete(fragShader);
	}
	
	GraphicsPipeline::~GraphicsPipeline()
	{
		
	}
}