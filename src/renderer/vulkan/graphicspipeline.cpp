#include "graphicspipeline.hpp"

#include "vertex.hpp"
#include "shader.hpp"

namespace Obtain::Graphics::Vulkan {
	GraphicsPipeline::GraphicsPipeline() {
		createRenderPass();
		createImageViews();
		
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
	
	void GraphicsPipeline::createImageViews() {
		swapchainImageViews.resize(swapchainImages.size());

		for (uint32_t i = 0; i < swapchainImages.size(); i++) {
			swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	void GraphicsPipeline::createRenderPass() {
		
		// Attachment for color
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
		// Attachment for depth buffering
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		// Attachment for multisampling
		VkAttachmentDescription colorAttachmentResolve = {};
		colorAttachmentResolve.format = swapchainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachmentResolveRef = {};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;
		
		std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}
}