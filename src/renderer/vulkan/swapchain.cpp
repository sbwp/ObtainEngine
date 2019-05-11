#include "swapchain.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "swapchain-support-details.hpp"
#include "shader.hpp"
#include "queue-family-indices.hpp"
#include "vertex.hpp"
#include "uniform-buffer-object.hpp"
#include "../../utils/time.hpp"

namespace Obtain::Graphics::Vulkan {

	/******************************************
	 ***************** public *****************
	 ******************************************/
	Swapchain::Swapchain(
		Device *device,
		std::array<uint32_t, 2> windowSize,
		QueueFamilyIndices indices,
		vk::UniqueCommandPool &commandPool,
		std::unique_ptr<Buffer> &vertexBuffer,
		std::unique_ptr<Buffer> &indexBuffer,
		std::unique_ptr<Image> &textureImage,
		vk::UniqueSampler &sampler
	)
		:
		device(device), commandPool(commandPool), vertexBuffer(vertexBuffer), indexBuffer(indexBuffer),
		textureImage(textureImage), sampler(sampler)
	{
		auto swapchainSupport = device->querySwapchainSupport();

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
		format = surfaceFormat.format;
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
		extent = chooseSwapExtent(
			swapchainSupport.capabilities,
			windowSize
		);

		swapchain = device->createSwapchain(surfaceFormat, extent, presentMode);
		images = device->getSwapchainImages(swapchain);
		imageViews = device->generateSwapchainImageViews(images, format);
		colorImage = Image::createColorImage(device, extent, format, commandPool);
		depthImage = Image::createDepthImage(device, extent, commandPool);
		renderPass = device->createRenderPass(format, depthImage->getFormat());
		descriptorSetLayout = device->createDescriptorSetLayout();
		pipelineLayout = device->createPipelineLayout(descriptorSetLayout);
		createPipeline();
		framebuffers = device->createFramebuffers(imageViews, colorImage->getView(),
		                                          depthImage->getView(), renderPass,
		                                          extent);
		createUniformBuffers();
		descriptorPool = device->createDescriptorPool(static_cast<uint32_t>(images.size()));
		descriptorSets = device->createDescriptorSets(static_cast<uint32_t>(images.size()),
		                                              descriptorSetLayout,
		                                              descriptorPool,
		                                              sampler,
		                                              textureImage->getView(),
		                                              uniformBuffers);
		createCommandBuffers();

		for (size_t i = 0; i < MaxFramesInFlight; i++) {
			imageReady[i] = device->createSemaphore();
			renderFinished[i] = device->createSemaphore();
			outOfFlight[i] = device->createFence(true);
		}
	}

	Swapchain::~Swapchain()
	{
		device->waitIdle();
		device->destroyImageViews(imageViews);
	}

	bool Swapchain::submitFrame(
		vk::Queue &graphicsQueue,
		vk::Queue &presentationQueue
	)
	{
		device->waitForFence(outOfFlight[currentFrame]);
		device->resetFence(outOfFlight[currentFrame]);
		uint32_t imageIndex;
		try {
			imageIndex = device->nextImage(swapchain, imageReady[currentFrame]);
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

	void Swapchain::createCommandBuffers()
	{
		commandBuffers = device->allocateCommandBuffers(commandPool,
		                                                vk::CommandBufferLevel::ePrimary,
		                                                static_cast<uint32_t>(framebuffers.size()));

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			auto &commandBuffer = commandBuffers[i];

			commandBuffer->begin(
				vk::CommandBufferBeginInfo(
					vk::CommandBufferUsageFlagBits::eSimultaneousUse,
					nullptr
				)
			);

			std::array<vk::ClearValue, 2> clearValues = {
				vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})),
				vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))
			};

			commandBuffer->beginRenderPass(
				vk::RenderPassBeginInfo(
					*renderPass,
					*(framebuffers[i]),
					vk::Rect2D(
						{0, 0},
						extent
					),
					clearValues.size(),
					clearValues.data()
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

		vk::PipelineShaderStageCreateInfo shaderCreateInfos[] = {
			vertShader->getCreateInfo(),
			fragShader->getCreateInfo()
		};

		pipeline = device->createGraphicsPipeline(extent, pipelineLayout, renderPass,
		                                          shaderCreateInfos);

		delete (vertShader);
		delete (fragShader);
	}

	void Swapchain::createUniformBuffers()
	{
		uniformBuffers.resize(images.size());
		for (size_t i = 0; i < images.size(); i++) {
			uniformBuffers[i] = std::make_unique<Buffer>(
				Buffer(
					device,
					sizeof(UniformBufferObject),
					vk::BufferUsageFlagBits::eUniformBuffer,
					vk::MemoryPropertyFlagBits::eHostVisible |
					vk::MemoryPropertyFlagBits::eHostCoherent)
			);
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
