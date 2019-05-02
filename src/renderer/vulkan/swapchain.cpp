#include "swapchain.hpp"

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "swapchain-support-details.hpp"
#include "shader.hpp"
#include "queue-family-indices.hpp"
#include "vertex2d.hpp"
#include "descriptor.hpp"
#include "uniform-buffer-object.hpp"
#include "../../utils/time.hpp"

namespace Obtain::Graphics::Vulkan {

	/******************************************
	 ***************** public *****************
	 ******************************************/
	Swapchain::Swapchain(
		vk::UniqueInstance &instance,
		std::unique_ptr<vk::PhysicalDevice> &physicalDevice,
		vk::UniqueDevice &device,
		vk::UniqueSurfaceKHR &surface,
		std::array<uint32_t, 2> windowSize,
		QueueFamilyIndices indices,
		vk::UniqueCommandPool &commandPool,
		std::unique_ptr<Buffer> &vertexBuffer,
		std::unique_ptr<Buffer> &indexBuffer
	)
		:
		instance(instance),
		device(device),
		physicalDevice(physicalDevice),
		surface(surface),
		commandPool(commandPool),
		vertexBuffer(vertexBuffer),
		indexBuffer(indexBuffer)
	{
		SwapchainSupportDetails swapchainSupport = SwapchainSupportDetails::querySwapchainSupport(
			*physicalDevice,
			surface
		);

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
		format = surfaceFormat.format;
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
		extent = chooseSwapExtent(
			swapchainSupport.capabilities,
			windowSize
		);

		uint32_t imageCount = swapchainSupport.capabilities
			                      .minImageCount + 1;
		if (swapchainSupport.capabilities
			    .maxImageCount > 0 &&
		    imageCount > swapchainSupport.capabilities
			    .maxImageCount
			) {
			imageCount = swapchainSupport.capabilities
				.maxImageCount;
		}

		vk::SharingMode sharingMode = vk::SharingMode::eConcurrent;
		uint32_t queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = {
			indices.graphicsFamily
				.value(), indices.presentFamily
				.value()
		};
		uint32_t *pQueueFamilyIndices = queueFamilyIndices;

		if (indices.graphicsFamily == indices.presentFamily) {
			sharingMode = vk::SharingMode::eExclusive;
			queueFamilyIndexCount = 0;
			pQueueFamilyIndices = nullptr;
		}

		swapchain = device->createSwapchainKHRUnique(
			vk::SwapchainCreateInfoKHR(
				vk::SwapchainCreateFlagsKHR(),
				*surface,
				imageCount,
				format,
				surfaceFormat.colorSpace,
				extent,
				1U,
				vk::ImageUsageFlagBits::eColorAttachment,
				sharingMode,
				queueFamilyIndexCount,
				pQueueFamilyIndices,
				swapchainSupport.capabilities
					.currentTransform,
				vk::CompositeAlphaFlagBitsKHR::eOpaque,
				presentMode,
				true,
				nullptr
			)
		);
		images = device->getSwapchainImagesKHR(*swapchain);
		imageViews.resize(images.size());

		for (size_t i = 0; i < images.size(); i++) {
			imageViews[i] = device->createImageView(
				vk::ImageViewCreateInfo(
					vk::ImageViewCreateFlags(),
					images[i],
					vk::ImageViewType::e2D,
					format,
					vk::ComponentMapping(
						vk::ComponentSwizzle::eIdentity, // r
						vk::ComponentSwizzle::eIdentity, // g
						vk::ComponentSwizzle::eIdentity, // b
						vk::ComponentSwizzle::eIdentity  // a
					),
					vk::ImageSubresourceRange(
						vk::ImageAspectFlagBits::eColor,
						0U, // base mip level
						1U, // level count
						0U, // base array level
						1U  // layer count
					)
				)
			);
		}
		createRenderPass();
		descriptorSetLayout = Descriptor::createDescriptorSetLayout(device);
		createPipeline();
		createFramebuffers();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();

		for (size_t i = 0; i < MaxFramesInFlight; i++) {
			imageReady[i] = device->createSemaphoreUnique(vk::SemaphoreCreateInfo());
			renderFinished[i] = device->createSemaphoreUnique(vk::SemaphoreCreateInfo());
			outOfFlight[i] = device->createFenceUnique(
				vk::FenceCreateInfo(
					vk::FenceCreateFlagBits::eSignaled
				)
			);
		}
	}

	Swapchain::~Swapchain()
	{
		device->waitIdle();
		for (auto imageView : imageViews) {
			device->destroyImageView(imageView);
		}
	}

	bool Swapchain::submitFrame(
		vk::Queue graphicsQueue,
		vk::Queue presentationQueue
	)
	{
		device->waitForFences(
			1,
			&outOfFlight[currentFrame].get(),
			true,
			std::numeric_limits<uint64_t>::max()
		);
		device->resetFences(1, &outOfFlight[currentFrame].get());
		uint32_t imageIndex;
		try {
			imageIndex = device->acquireNextImageKHR(
				*swapchain,
				std::numeric_limits<uint64_t>::max(),
				*imageReady[currentFrame],
				nullptr
			).value;
		} catch (vk::OutOfDateKHRError &error) {
			return false;
		}

		updateUniformBuffer(imageIndex);

		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::SubmitInfo submitInfo(
			1,
			&imageReady[currentFrame].get(),
			waitStages,
			1,
			&commandBuffers[imageIndex].get(),
			1,
			&renderFinished[currentFrame].get()
		);

		graphicsQueue.submit(1, &submitInfo, *outOfFlight[currentFrame]);

		vk::Result result;
		try {
			result = presentationQueue.presentKHR(
				vk::PresentInfoKHR(
					1,
					&renderFinished[currentFrame].get(),
					1,
					&swapchain.get(),
					&imageIndex,
					nullptr
				)
			);
		} catch (vk::OutOfDateKHRError &error) {
			currentFrame = (currentFrame + 1) % MaxFramesInFlight;
			return false;
		}

		currentFrame = (currentFrame + 1) % MaxFramesInFlight;
		return result == vk::Result::eSuccess;
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR> &availableFormats
	)
	{
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
			return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
		}
		for (const auto &availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
			    availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
				) {
				return availableFormat;
			}
		}
		// Anything else is ok if the preferred isn't there, so just pick the first format
		return availableFormats[0];
	}

	vk::PresentModeKHR Swapchain::chooseSwapPresentMode(
		const std::vector<vk::PresentModeKHR> &availablePresentModes
	)
	{
		vk::PresentModeKHR best = vk::PresentModeKHR::eFifo;

		for (const auto &availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				best = availablePresentMode;
			}
		}

		return best;
	}

	vk::Extent2D Swapchain::chooseSwapExtent(
		const vk::SurfaceCapabilitiesKHR &capabilities,
		std::array<uint32_t, 2> windowSize
	)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			vk::Extent2D actualExtent = {windowSize[0], windowSize[1]};

			actualExtent.width = std::max(
				capabilities.minImageExtent
					.width,
				std::min(
					capabilities.maxImageExtent
						.width,
					actualExtent.width
				)
			);

			actualExtent.height = std::max(
				capabilities.minImageExtent
					.height,
				std::min(
					capabilities.maxImageExtent
						.height,
					actualExtent.height
				)
			);

			return actualExtent;
		}
	}

	void Swapchain::createPipeline()
	{
		Shader *vertShader = new Shader(
			device,
			"assets/shaders/vert.spv",
			vk::ShaderStageFlagBits::eVertex
		);
		Shader *fragShader = new Shader(
			device,
			"assets/shaders/frag.spv",
			vk::ShaderStageFlagBits::eFragment
		);

		auto bindingDescription = Vertex2D::getBindingDescription();
		auto attributeDescriptions = Vertex2D::getAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo(
			vk::PipelineVertexInputStateCreateFlags(),
			1,
			&bindingDescription,
			static_cast<uint32_t>(attributeDescriptions.size()),
			attributeDescriptions.data()
		);

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList,
			false
		);

		vk::Viewport viewport(
			0.0f,
			0.0f,
			(float) extent.width,
			(float) extent.height,
			0.0f,
			0.0f
		);

		vk::Rect2D scissor(
			vk::Offset2D(
				0,
				0
			),
			extent
		);

		vk::PipelineViewportStateCreateInfo viewportStateCreateInfo(
			vk::PipelineViewportStateCreateFlags(),
			1,
			&viewport,
			1,
			&scissor
		);

		vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo(
			vk::PipelineRasterizationStateCreateFlags(),
			false,
			false,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eCounterClockwise,
			false,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		);

		// temporarily disabled
		vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo(
			vk::PipelineMultisampleStateCreateFlags(),
			vk::SampleCountFlagBits::e1
		);

		vk::PipelineColorBlendAttachmentState colorBlendAttachmentState(
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

		vk::PipelineColorBlendStateCreateInfo colorBlendingCreateInfo(
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

		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo(
			vk::PipelineDynamicStateCreateFlags(),
			static_cast<uint32_t>(dynamicStates.size()),
			dynamicStates.data()
		);

		pipelineLayout = device->createPipelineLayoutUnique(
			vk::PipelineLayoutCreateInfo(
				vk::PipelineLayoutCreateFlags(),
				1,
				&descriptorSetLayout.get(),
				0,
				nullptr
			)
		);

		vk::PipelineShaderStageCreateInfo shaderCreateInfos[] = {
			vertShader->getCreateInfo(),
			fragShader->getCreateInfo()
		};

		pipeline = device->createGraphicsPipelineUnique(
			nullptr,
			vk::GraphicsPipelineCreateInfo(
				vk::PipelineCreateFlags(),
				2,
				shaderCreateInfos,
				&vertexInputStateCreateInfo,
				&inputAssemblyCreateInfo,
				nullptr,
				&viewportStateCreateInfo,
				&rasterizerCreateInfo,
				&multisamplingCreateInfo,
				nullptr,
				&colorBlendingCreateInfo,
				nullptr,
				*pipelineLayout,
				*renderPass,
				0
			)
		);

		delete (vertShader);
		delete (fragShader);
	}

	void Swapchain::createRenderPass()
	{
		vk::AttachmentDescription colorAttachment(
			vk::AttachmentDescriptionFlags(),
			format,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR
		);

		vk::AttachmentReference colorAttachmentRef(
			0,
			vk::ImageLayout::eColorAttachmentOptimal
		);

		vk::SubpassDescription subpassDescription(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			0,
			nullptr,
			1,
			&colorAttachmentRef
		);

		vk::SubpassDependency dependency(
			VK_SUBPASS_EXTERNAL,
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlags(),
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::DependencyFlags()
		);

		renderPass = device->createRenderPassUnique(
			vk::RenderPassCreateInfo(
				vk::RenderPassCreateFlags(),
				1,
				&colorAttachment,
				1,
				&subpassDescription,
				1,
				&dependency
			)
		);
	}

	void Swapchain::createFramebuffers()
	{
		size_t size = imageViews.size();

		// Resize up front to avoid multiple resizings
		framebuffers.resize(size);

		for (size_t i = 0; i < size; i++) {
			framebuffers[i] = device->createFramebufferUnique(
				vk::FramebufferCreateInfo(
					vk::FramebufferCreateFlags(),
					*renderPass,
					1,
					&imageViews[i],
					extent.width,
					extent.height,
					1
				)
			);
		}
	}

	void Swapchain::createCommandBuffers()
	{
		commandBuffers = device->allocateCommandBuffersUnique(
			vk::CommandBufferAllocateInfo(
				*commandPool,
				vk::CommandBufferLevel::ePrimary,
				(uint32_t) framebuffers.size()
			)
		);

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			auto &commandBuffer = commandBuffers[i];

			commandBuffer->begin(
				vk::CommandBufferBeginInfo(
					vk::CommandBufferUsageFlagBits::eSimultaneousUse,
					nullptr
				)
			);

			vk::ClearValue clearColor(
				vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})
			);

			commandBuffer->beginRenderPass(
				vk::RenderPassBeginInfo(
					*renderPass,
					*(framebuffers[i]),
					vk::Rect2D(
						{0, 0},
						extent
					),
					1,
					&clearColor
				),
				vk::SubpassContents::eInline
			);

			commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
			vk::Buffer vertexBuffers[] = {*(vertexBuffer->getBuffer())};
			vk::DeviceSize offsets[] = {0};
			commandBuffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
			commandBuffer->bindIndexBuffer(*(indexBuffer->getBuffer()),
			                               static_cast<vk::DeviceSize>(0),
			                               vk::IndexType::eUint32);
			commandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			                                      *pipelineLayout,
			                                      0,
			                                      1,
			                                      &descriptorSets[i].get(),
			                                      0,
			                                      nullptr);
			commandBuffer->drawIndexed(static_cast<uint32_t>(indexBuffer->getSize() / sizeof(uint32_t)),
			                           1, 0, 0, 0);
			commandBuffer->endRenderPass();
			commandBuffer->end();
		}
	}

	void Swapchain::createUniformBuffers()
	{
		uniformBuffers.resize(images.size());
		for (size_t i = 0; i < images.size(); i++) {
			uniformBuffers[i] = std::make_unique<Buffer>(
				Buffer(
					device,
					physicalDevice,
					sizeof(UniformBufferObject),
					vk::BufferUsageFlagBits::eUniformBuffer,
					vk::MemoryPropertyFlagBits::eHostVisible |
					vk::MemoryPropertyFlagBits::eHostCoherent)
			);
		}
	}

	void Swapchain::createDescriptorPool()
	{
		vk::DescriptorPoolSize size(vk::DescriptorType::eUniformBuffer,
		                                static_cast<uint32_t>(images.size()));

		vk::DescriptorPoolCreateInfo createInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		                                        static_cast<uint32_t>(images.size()),
		                                        1,
		                                        &size);

		descriptorPool = device->createDescriptorPoolUnique(createInfo);
	}

	void Swapchain::createDescriptorSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(images.size(), *descriptorSetLayout);

		vk::DescriptorSetAllocateInfo allocateInfo(*descriptorPool,
		                                           static_cast<uint32_t>(images.size()),
		                                           layouts.data());

		descriptorSets = device->allocateDescriptorSetsUnique(allocateInfo);

		for (size_t i = 0; i < images.size(); i++) {
			vk::DescriptorBufferInfo bufferInfo(*(uniformBuffers[i]->getBuffer()),
			                                    0,
			                                    sizeof(UniformBufferObject));

			vk::WriteDescriptorSet writeOp(*(descriptorSets[i]),
			                               0,
			                               0,
			                               1,
			                               vk::DescriptorType::eUniformBuffer,
			                               nullptr,
			                               &bufferInfo,
			                               nullptr);

			device->updateDescriptorSets(1,
			                             &writeOp,
			                             0,
			                             nullptr);
		}
	}

	void Swapchain::updateUniformBuffer(uint32_t currentImage)
	{
		float time = Time::elapsedTime();
		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f),
		                        time * glm::radians(90.0f),
		                        glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
		                       glm::vec3(0.0f, 0.0f, 0.0f),
		                       glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.projection = glm::perspective(glm::radians(45.0f),
		                                  static_cast<float>(extent.width) / static_cast<float>(extent.height),
		                                  01.f,
		                                  10.0f);
		ubo.projection[1][1] *= -1;

		uniformBuffers[currentImage]->load(0, &ubo, sizeof(ubo));
	}
}
