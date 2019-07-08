#include "device.hpp"

#include <vector>
#include <set>
#include <map>
#include <memory>
#include <iostream>
#include <limits>

#include "swapchain-support-details.hpp"
#include "queue-family-indices.hpp"
#include "validation.hpp"
#include "vertex.hpp"
#include "shader.hpp"
#include "uniform-buffer-object.hpp"
#include "buffer.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/

	Device::Device(const std::string &gameTitle,
	               std::array<uint32_t, 3> gameVersion,
	               std::array<uint32_t, 3> engineVersion)
		: gameVersion(gameVersion),
		  engineVersion(engineVersion),
		  gameTitle(gameTitle),
		  resizeOccurred(false)
	{
		allocator = std::make_unique<Allocator>(Allocator(this));
		windowSize = {1600, 900};
		createWindow();
		createInstance();

		surface = createSurface(
			*instance,
			window
		);

		physicalDevice = selectPhysicalDevice(
			instance
		);

		findSampleCount();

		queueFamilyIndices = findQueueFamilies();

		device = createLogicalDevice(
			instance
		);

		graphicsQueue = device->getQueue(queueFamilyIndices.graphicsFamily.value(), 0);
		presentQueue = device->getQueue(queueFamilyIndices.presentFamily.value(), 0);
	}

	Device::~Device()
	{
		instance->destroyDebugUtilsMessengerEXT(
			debugMessenger,
			nullptr,
			loader
		);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	QueueFamilyIndices Device::getQueueFamilies()
	{
		return queueFamilyIndices;
	}

	vk::Queue *Device::getGraphicsQueue()
	{
		return &graphicsQueue;
	}

	vk::Queue *Device::getPresentQueue()
	{
		return &presentQueue;
	}

	SwapchainSupportDetails Device::querySwapchainSupport()
	{
		return querySwapchainSupport(physicalDevice);
	}

	SwapchainSupportDetails Device::querySwapchainSupport(vk::PhysicalDevice physicalDeviceCandidate)
	{
		SwapchainSupportDetails details;

		details.capabilities = physicalDeviceCandidate.getSurfaceCapabilitiesKHR(*surface);

		// Check size of formats
		details.formats = physicalDeviceCandidate.getSurfaceFormatsKHR(*surface);

		details.presentModes = physicalDeviceCandidate.getSurfacePresentModesKHR(*surface);

		return details;
	}

	vk::UniqueSwapchainKHR Device::createSwapchain(
		vk::SurfaceFormatKHR surfaceFormat,
		const vk::Extent2D &extent,
		vk::PresentModeKHR presentMode)
	{
		SwapchainSupportDetails support = querySwapchainSupport();

		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
			imageCount = support.capabilities.maxImageCount;
		}

		vk::SharingMode sharingMode = vk::SharingMode::eConcurrent;
		uint32_t queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndexList[] = {
			queueFamilyIndices.graphicsFamily.value(),
			queueFamilyIndices.presentFamily.value()
		};
		uint32_t *pQueueFamilyIndices = queueFamilyIndexList;

		if (queueFamilyIndices.graphicsFamily == queueFamilyIndices.presentFamily) {
			sharingMode = vk::SharingMode::eExclusive;
			queueFamilyIndexCount = 0;
			pQueueFamilyIndices = nullptr;
		}

		return device->createSwapchainKHRUnique(
			vk::SwapchainCreateInfoKHR(
				vk::SwapchainCreateFlagsKHR(),
				*surface,
				static_cast<uint32_t>(imageCount),
				surfaceFormat.format,
				surfaceFormat.colorSpace,
				extent,
				1U,
				vk::ImageUsageFlagBits::eColorAttachment,
				sharingMode,
				queueFamilyIndexCount,
				pQueueFamilyIndices,
				support.capabilities
					.currentTransform,
				vk::CompositeAlphaFlagBitsKHR::eOpaque,
				presentMode,
				true,
				nullptr
			)
		);
	}

	std::vector<vk::Image> Device::getSwapchainImages(vk::UniqueSwapchainKHR &swapchain)
	{
		return device->getSwapchainImagesKHR(*swapchain);
	}

	std::vector<vk::ImageView> Device::generateSwapchainImageViews(std::vector<vk::Image> &images,
	                                                               const vk::Format &format)
	{
		std::vector<vk::ImageView> imageViews;
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
		return imageViews;
	}

	void Device::destroyImageViews(std::vector<vk::ImageView> imageViews)
	{
		for (auto imageView : imageViews) {
			device->destroyImageView(imageView);
		}
	}

	uint32_t Device::nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueSemaphore &triggerSemaphore)
	{
		return device->acquireNextImageKHR(
			*swapchain,
			std::numeric_limits<uint64_t>::max(),
			*triggerSemaphore,
			nullptr
		).value;
	}

	uint32_t Device::nextImage(vk::UniqueSwapchainKHR &swapchain, vk::UniqueFence &triggerFence)
	{
		return device->acquireNextImageKHR(
			*swapchain,
			std::numeric_limits<uint64_t>::max(),
			nullptr,
			*triggerFence
		).value;
	}

	vk::UniqueImage Device::createImage(const vk::Extent3D &extent, const vk::Format &format, uint32_t mipLevels,
	                                    const vk::ImageTiling &tiling, const vk::ImageUsageFlags &usageFlags,
	                                    vk::SampleCountFlagBits sampleCount)
	{
		vk::ImageCreateInfo createInfo(vk::ImageCreateFlags(),
		                               vk::ImageType::e2D,
		                               format,
		                               extent,
		                               mipLevels,
		                               1u,
		                               sampleCount,
		                               tiling,
		                               usageFlags,
		                               vk::SharingMode::eExclusive,
		                               0u,
		                               nullptr,
		                               vk::ImageLayout::eUndefined);

		return device->createImageUnique(createInfo);
	}

	vk::UniqueImageView Device::createImageView(vk::UniqueImage &image, const vk::Format &format, uint32_t mipLevels,
	                                            const vk::ImageAspectFlags &aspectMask)
	{
		return device->createImageViewUnique(
			vk::ImageViewCreateInfo(
				vk::ImageViewCreateFlags(),
				*image,
				vk::ImageViewType::e2D,
				format,
				vk::ComponentMapping(
					vk::ComponentSwizzle::eIdentity, // r
					vk::ComponentSwizzle::eIdentity, // g
					vk::ComponentSwizzle::eIdentity, // b
					vk::ComponentSwizzle::eIdentity  // a
				),
				vk::ImageSubresourceRange(
					aspectMask,
					0U, // base mip level
					mipLevels, // level count
					0U, // base array level
					1U  // layer count
				)
			)
		);
	}

	vk::UniqueSampler Device::createSampler(float mipLevels)
	{
		vk::SamplerCreateInfo createInfo(vk::SamplerCreateFlags(),
		                                 vk::Filter::eLinear,
		                                 vk::Filter::eLinear,
		                                 vk::SamplerMipmapMode::eLinear,
		                                 vk::SamplerAddressMode::eRepeat,
		                                 vk::SamplerAddressMode::eRepeat,
		                                 vk::SamplerAddressMode::eRepeat,
		                                 0.0f,
		                                 true,
		                                 16,
		                                 false,
		                                 vk::CompareOp::eAlways,
		                                 0.0f,
		                                 mipLevels,
		                                 vk::BorderColor::eIntOpaqueBlack,
		                                 false);
		return device->createSamplerUnique(createInfo);
	}

	vk::FormatProperties Device::getFormatProperties(const vk::Format &format)
	{
		return physicalDevice.getFormatProperties(format);
	}

	vk::MemoryRequirements Device::getImageMemoryRequirements(vk::UniqueImage &image)
	{
		return device->getImageMemoryRequirements(*image);
	}

	void Device::bindImageMemory(vk::UniqueImage &image, vk::UniqueDeviceMemory &memory, uint32_t offset)
	{
		device->bindImageMemory(
			*image,
			*memory,
			offset
		);
	}

	vk::UniqueDescriptorSetLayout Device::createDescriptorSetLayout()
	{
		std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {
			vk::DescriptorSetLayoutBinding(
				0,
				vk::DescriptorType::eUniformBuffer,
				1,
				vk::ShaderStageFlagBits::eVertex,
				nullptr
			),
			vk::DescriptorSetLayoutBinding(
				1,
				vk::DescriptorType::eCombinedImageSampler,
				1,
				vk::ShaderStageFlagBits::eFragment,
				nullptr
			)
		};

		return device->createDescriptorSetLayoutUnique(
			vk::DescriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				bindings.size(),
				bindings.data()
			)
		);
	}

	vk::UniqueDescriptorPool Device::createDescriptorPool(uint32_t size)
	{
		std::array<vk::DescriptorPoolSize, 2> poolSizes = {
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer,
			                       size),
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler,
			                       size)
		};

		vk::DescriptorPoolCreateInfo createInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		                                        size,
		                                        poolSizes.size(),
		                                        poolSizes.data());

		return device->createDescriptorPoolUnique(createInfo);
	}


	std::vector<vk::UniqueDescriptorSet> Device::createDescriptorSets(uint32_t size,
	                                                                  vk::UniqueDescriptorSetLayout &descriptorSetLayout,
	                                                                  vk::UniqueDescriptorPool &descriptorPool,
	                                                                  vk::UniqueSampler &sampler,
	                                                                  vk::UniqueImageView &imageView,
	                                                                  std::vector<std::unique_ptr<Buffer>> &uniformBuffers)
	{
		std::vector<vk::DescriptorSetLayout> layouts(size, *descriptorSetLayout);

		vk::DescriptorSetAllocateInfo allocateInfo(*descriptorPool,
		                                           static_cast<uint32_t>(size),
		                                           layouts.data());

		auto descriptorSets = device->allocateDescriptorSetsUnique(allocateInfo);

		for (size_t i = 0; i < size; i++) {
			vk::DescriptorBufferInfo bufferInfo(*(uniformBuffers[i]->getBuffer()),
			                                    0,
			                                    sizeof(UniformBufferObject));

			vk::DescriptorImageInfo imageInfo(*sampler, *imageView,
			                                  vk::ImageLayout::eShaderReadOnlyOptimal);

			std::array<vk::WriteDescriptorSet, 2> writeOps = {
				vk::WriteDescriptorSet(*(descriptorSets[i]),
				                       0,
				                       0,
				                       1,
				                       vk::DescriptorType::eUniformBuffer,
				                       nullptr,
				                       &bufferInfo,
				                       nullptr),
				vk::WriteDescriptorSet(*(descriptorSets[i]),
				                       1,
				                       0,
				                       1,
				                       vk::DescriptorType::eCombinedImageSampler,
				                       &imageInfo),
			};

			device->updateDescriptorSets(writeOps, nullptr);
		}
		return descriptorSets;
	}

	vk::UniqueSemaphore Device::createSemaphore()
	{
		return device->createSemaphoreUnique(vk::SemaphoreCreateInfo());
	}

	vk::UniqueFence Device::createFence(bool signaled)
	{
		vk::FenceCreateFlags flags = signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags();
		return device->createFenceUnique(vk::FenceCreateInfo(flags));
	}

	void Device::waitForFence(vk::UniqueFence &fence)
	{
		device->waitForFences(
			1,
			&fence.get(),
			true,
			std::numeric_limits<uint64_t>::max()
		);
	}

	void Device::resetFence(vk::UniqueFence &fence)
	{
		device->resetFences(1, &fence.get());
	}

	vk::UniqueShaderModule Device::createShaderModule(size_t size, void *data)
	{
		return device->createShaderModuleUnique(
			vk::ShaderModuleCreateInfo(
				vk::ShaderModuleCreateFlags(),
				size,
				reinterpret_cast<uint32_t *>(data)
			)
		);
	}

	vk::UniquePipelineLayout Device::createPipelineLayout(vk::UniqueDescriptorSetLayout &descriptorSetLayout)
	{
		return device->createPipelineLayoutUnique(
			vk::PipelineLayoutCreateInfo(
				vk::PipelineLayoutCreateFlags(),
				1,
				&descriptorSetLayout.get(),
				0,
				nullptr
			)
		);
	}

	vk::UniquePipeline Device::createGraphicsPipeline(const vk::Extent2D &extent,
	                                                  vk::UniquePipelineLayout &pipelineLayout,
	                                                  vk::UniqueRenderPass &renderPass,
	                                                  vk::PipelineShaderStageCreateInfo *shaderCreateInfos)
	{
		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

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
			1.0f
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
			sampleCount,
			true,
			0.2f
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

		vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo(vk::PipelineDepthStencilStateCreateFlags(),
		                                                                    true,
		                                                                    true,
		                                                                    vk::CompareOp::eLess,
		                                                                    false,
		                                                                    false,
		                                                                    vk::StencilOpState(),
		                                                                    vk::StencilOpState(),
		                                                                    0.0f,
		                                                                    1.0f);

		return device->createGraphicsPipelineUnique(
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
				&depthStencilStateCreateInfo,
				&colorBlendingCreateInfo,
				nullptr,
				*pipelineLayout,
				*renderPass,
				0
			)
		);
	}

	vk::UniqueRenderPass Device::createRenderPass(const vk::Format &colorFormat, const vk::Format &depthFormat)
	{
		std::array<vk::AttachmentDescription, 3> attachmentDescriptions = {
			vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
			                          colorFormat,
			                          sampleCount,
			                          vk::AttachmentLoadOp::eClear,
			                          vk::AttachmentStoreOp::eStore,
			                          vk::AttachmentLoadOp::eDontCare,
			                          vk::AttachmentStoreOp::eDontCare,
			                          vk::ImageLayout::eUndefined,
			                          vk::ImageLayout::eColorAttachmentOptimal
			),
			vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
			                          depthFormat,
			                          sampleCount,
			                          vk::AttachmentLoadOp::eClear,
			                          vk::AttachmentStoreOp::eDontCare,
			                          vk::AttachmentLoadOp::eDontCare,
			                          vk::AttachmentStoreOp::eDontCare,
			                          vk::ImageLayout::eUndefined,
			                          vk::ImageLayout::eDepthStencilAttachmentOptimal
			),
			vk::AttachmentDescription(vk::AttachmentDescriptionFlags(),
			                          colorFormat,
			                          vk::SampleCountFlagBits::e1,
			                          vk::AttachmentLoadOp::eDontCare,
			                          vk::AttachmentStoreOp::eStore,
			                          vk::AttachmentLoadOp::eDontCare,
			                          vk::AttachmentStoreOp::eDontCare,
			                          vk::ImageLayout::eUndefined,
			                          vk::ImageLayout::ePresentSrcKHR
			)
		};

		vk::AttachmentReference colorAttachmentRef(
			0,
			vk::ImageLayout::eColorAttachmentOptimal
		);

		vk::AttachmentReference depthStencilAttachmentRef(
			1,
			vk::ImageLayout::eDepthStencilAttachmentOptimal
		);

		vk::AttachmentReference colorAttachmentResolveRef(
			2,
			vk::ImageLayout::eColorAttachmentOptimal
		);

		vk::SubpassDescription subpassDescription(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			0,
			nullptr,
			1,
			&colorAttachmentRef,
			&colorAttachmentResolveRef,
			&depthStencilAttachmentRef
		);

		vk::SubpassDependency dependency(
			(VK_SUBPASS_EXTERNAL),
			0,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlags(),
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::DependencyFlags()
		);

		return device->createRenderPassUnique(
			vk::RenderPassCreateInfo(
				vk::RenderPassCreateFlags(),
				attachmentDescriptions.size(),
				attachmentDescriptions.data(),
				1,
				&subpassDescription,
				1,
				&dependency
			)
		);
	}

	std::vector<vk::UniqueFramebuffer> Device::createFramebuffers(std::vector<vk::ImageView> imageViews,
	                                                              vk::UniqueImageView &colorImageView,
	                                                              vk::UniqueImageView &depthImageView,
	                                                              vk::UniqueRenderPass &renderPass,
	                                                              const vk::Extent2D &extent)
	{
		size_t size = imageViews.size();

		// Resize up front to avoid multiple resizings
		std::vector<vk::UniqueFramebuffer> framebuffers;
		framebuffers.resize(size);

		for (size_t i = 0; i < size; i++) {
			std::array<vk::ImageView, 3> attachments = {
				*colorImageView,
				*depthImageView,
				imageViews[i]
			};

			framebuffers[i] = device->createFramebufferUnique(
				vk::FramebufferCreateInfo(
					vk::FramebufferCreateFlags(),
					*renderPass,
					attachments.size(),
					attachments.data(),
					extent.width,
					extent.height,
					1
				)
			);
		}
		return framebuffers;
	}

	std::vector<vk::UniqueCommandBuffer> Device::allocateCommandBuffers(vk::UniqueCommandPool &commandPool,
	                                                                    vk::CommandBufferLevel level,
	                                                                    uint32_t count)
	{
		return device->allocateCommandBuffersUnique(
			vk::CommandBufferAllocateInfo(
				*commandPool,
				level,
				count
			)
		);
	}


	vk::UniqueCommandPool Device::createCommandPool()
	{
		return device->createCommandPoolUnique(
			vk::CommandPoolCreateInfo(
				vk::CommandPoolCreateFlags(),
				queueFamilyIndices.graphicsFamily.value()
			)
		);
	}

	void Device::waitIdle()
	{
		device->waitIdle();
	}

	vk::UniqueBuffer Device::createBuffer(vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags)
	{
		return device->createBufferUnique(vk::BufferCreateInfo(vk::BufferCreateFlags(),
		                                                       size,
		                                                       usageFlags,
		                                                       vk::SharingMode::eExclusive));
	}

	uint32_t Device::findMemoryType(uint32_t typeFilter, const vk::MemoryPropertyFlags &properties)
	{
		auto memoryProperties = physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1u << i)) &&
			    memoryProperties.memoryTypes[i].propertyFlags.operator&(properties) == properties
				) {
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	vk::UniqueDeviceMemory Device::allocateMemory(vk::DeviceSize size, uint32_t memoryType)
	{
		return device->allocateMemoryUnique(
			vk::MemoryAllocateInfo(
				size,
				memoryType
			)
		);
	}


	vk::MemoryRequirements Device::getBufferMemoryRequirements(vk::UniqueBuffer &buffer)
	{
		return device->getBufferMemoryRequirements(*buffer);
	}

	void Device::bindBufferMemory(vk::UniqueBuffer &buffer, vk::UniqueDeviceMemory &memory, uint32_t offset)
	{
		device->bindBufferMemory(
			*buffer,
			*memory,
			offset
		);
	}

	void *Device::mapMemory(vk::UniqueDeviceMemory &memory, uint32_t offset, vk::DeviceSize size)
	{
		return device->mapMemory(*memory, offset, size);
	}

	void Device::unmapMemory(vk::UniqueDeviceMemory &memory)
	{
		device->unmapMemory(*memory);
	}

	void Device::setMemory(vk::UniqueDeviceMemory &memory, uint32_t offset, vk::DeviceSize size, void *src)
	{
		void *dest = mapMemory(memory, offset, size);

		memcpy(
			dest,
			src,
			static_cast<size_t>(size));

		unmapMemory(memory);
	}

	bool Device::windowOpen()
	{
		return !glfwWindowShouldClose(window);
	}

	std::array<uint32_t, 2> Device::updateWindowSizeOnceVisible()
	{
		int width = 0, height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		setWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	std::array<uint32_t, 2> Device::getWindowSize()
	{
		return std::array<uint32_t, 2>(windowSize);
	}

	void Device::setWindowSize(uint32_t width, uint32_t height)
	{
		windowSize[0] = width;
		windowSize[1] = height;
	}

	void Device::resetResizeFlag()
	{
		resizeOccurred = false;
	}

	bool Device::getResizeFlag()
	{
		return resizeOccurred;
	}

	bool Device::hasOptimalTilingFeature(const vk::Format &format, const vk::FormatFeatureFlags &feature)
	{
		return static_cast<bool>(physicalDevice.getFormatProperties(format).optimalTilingFeatures & feature);
	}

	vk::SampleCountFlagBits Device::getSampleCount()
	{
		return sampleCount;
	}


	/******************************************
	 ***************** private *****************
	 ******************************************/



	void Device::findSampleCount()
	{
		auto properties = physicalDevice.getProperties();

		vk::SampleCountFlags counts = min(properties.limits.framebufferColorSampleCounts,
		                                  properties.limits.framebufferDepthSampleCounts);

		std::array<vk::SampleCountFlagBits, 6> options = {
			vk::SampleCountFlagBits::e64, vk::SampleCountFlagBits::e32, vk::SampleCountFlagBits::e16,
			vk::SampleCountFlagBits::e8, vk::SampleCountFlagBits::e4, vk::SampleCountFlagBits::e2
		};

		for (auto option : options) {
			if (counts & option) {
				sampleCount = option;
				return;
			}
		}

		sampleCount = vk::SampleCountFlagBits::e1;
	}

	QueueFamilyIndices Device::findQueueFamilies()
	{
		return findQueueFamilies(physicalDevice);
	}

	QueueFamilyIndices Device::findQueueFamilies(const vk::PhysicalDevice &physicalDeviceCandidate)
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDeviceCandidate.getQueueFamilyProperties();

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {

			vk::Bool32 presentSupport = false;
			physicalDeviceCandidate.getSurfaceSupportKHR(
				i,
				*surface,
				&presentSupport
			);

			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	vk::PhysicalDevice Device::selectPhysicalDevice(
		vk::UniqueInstance &instance
	)
	{
		std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();
		if (devices.empty()) {
			throw std::runtime_error("no supported GPUs found");
		}

		std::multimap<uint32_t, vk::PhysicalDevice> deviceMap;

		for (const auto &device : devices) {
			deviceMap.insert(
				std::make_pair(
					ratePhysicalDeviceSuitability(
						device
					),
					device
				));
		}

		if (deviceMap.rbegin()->first > 0) {
			return deviceMap.rbegin()->second;
		} else {
			throw std::runtime_error("no suitable GPUs found");
		}
	}

	vk::UniqueDevice Device::createLogicalDevice(
		vk::UniqueInstance &instance
	)
	{
		const float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			queueFamilyIndices.graphicsFamily
				.value(),
			queueFamilyIndices.presentFamily
				.value()
		};

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			queueCreateInfos.emplace_back(
				vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(),
					queueFamily,
					1U,
					&queuePriority
				)
			);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
		deviceFeatures.samplerAnisotropy = true;
		deviceFeatures.sampleRateShading = true;
		std::vector<const char *> validationLayers = Validation::getValidationLayers();

		return physicalDevice.createDeviceUnique(
			vk::DeviceCreateInfo(
				vk::DeviceCreateFlags(),
				static_cast<uint32_t>(queueCreateInfos.size()),
				queueCreateInfos.data(),
				static_cast<uint32_t>(validationLayers.size()),
				validationLayers.empty() ? (const char *const *) nullptr : validationLayers.data(),
				static_cast<uint32_t>(deviceExtensions.size()),
				deviceExtensions.data(),
				&deviceFeatures
			)
		);
	}

	vk::UniqueSurfaceKHR Device::createSurface(const vk::Instance &instance, GLFWwindow *window)
	{
		VkSurfaceKHR surface;

		if (glfwCreateWindowSurface(
			instance,
			window,
			nullptr,
			&surface
		) != VK_SUCCESS) {
			throw std::runtime_error("could not create surface");
		}
		return vk::UniqueSurfaceKHR(
			surface,
			instance
		);
	}

	const std::vector<const char *> Device::deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	uint32_t Device::ratePhysicalDeviceSuitability(const vk::PhysicalDevice &physicalDeviceCandidate)
	{
		// Get device properties
		auto deviceProperties = physicalDeviceCandidate.getProperties();

		// Get device features
		auto deviceFeatures = physicalDeviceCandidate.getFeatures();

		uint32_t score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits
			.maxImageDimension2D;

		// Check if required Extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(physicalDeviceCandidate);

		// Check for sufficient swap chains
		bool swapchainAdequate = false;
		if (extensionsSupported) {
			SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDeviceCandidate);
			swapchainAdequate = !swapchainSupport.formats
				.empty() && !swapchainSupport.presentModes
				.empty();
		}

		// if (!deviceFeatures.samplerAnisotropy) {
		// TODO: instead of disqualifying for missing Anistropy below, conditionally use it
		// 	score -= 200;
		// }

		// Check for missing features that are complete dealbreakers
		if (!(deviceFeatures.geometryShader && findQueueFamilies(physicalDeviceCandidate).isComplete() &&
		      extensionsSupported && swapchainAdequate && deviceFeatures.samplerAnisotropy)) {
			return 0;
		}

		return score;
	}

	bool Device::checkDeviceExtensionSupport(const vk::PhysicalDevice &physicalDeviceCandidate)
	{

		std::vector<vk::ExtensionProperties> availableExtensions;
		availableExtensions = physicalDeviceCandidate.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(
			deviceExtensions.begin(),
			deviceExtensions.end());

		for (const auto &extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	bool Device::checkForSupportedExtensions(std::vector<const char *> requiredExtensions)
	{
		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

		// Print all available extensions
		std::cout << "available extensions:" << std::endl;
		for (const auto &extension : extensions) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		// Check each required extension for availability
		for (const auto &requiredExtension : requiredExtensions) {
			bool found = false;
			for (const auto &extension : extensions) {
				if (strcmp(
					extension.extensionName,
					requiredExtension
				) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				std::cerr << "Required Extension " << requiredExtension << " not found." << std::endl;
				return false;
			}
		}
		return true;
	}

	std::vector<const char *> Device::getRequiredExtensions(bool useValidationLayers)
	{
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(
			glfwExtensions,
			glfwExtensions + glfwExtensionCount
		);

		if (useValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void Device::createInstance()
	{
		vk::ApplicationInfo appInfo(
			gameTitle.c_str(),
			VK_MAKE_VERSION(gameVersion[0],
			                gameVersion[1],
			                gameVersion[2]),
			"Obtain Engine",
			VK_MAKE_VERSION(engineVersion[0],
			                engineVersion[1],
			                engineVersion[2]), // engine gameVersion (replace with not constant)
			VK_API_VERSION_1_1
		);

		bool useValidationLayers = Validation::useValidation();
		std::vector<const char *> requiredExtensions = getRequiredExtensions(useValidationLayers);
		if (!checkForSupportedExtensions(requiredExtensions)) {
			throw std::runtime_error("Required Extensions not supported");
		}

		instance = vk::createInstanceUnique(
			vk::InstanceCreateInfo(
				vk::InstanceCreateFlags(),
				&appInfo,
				useValidationLayers ? static_cast<uint32_t>(Validation::validationLayers.size()) : 0U,
				useValidationLayers ? Validation::validationLayers.data() : nullptr,
				static_cast<uint32_t>(requiredExtensions.size()),
				requiredExtensions.data()
			)
		);

		loader.init(*instance);

		debugMessenger = Validation::createDebugMessenger(
			instance,
			loader
		);
	}

	void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto p_resizeOccurred = reinterpret_cast<bool *>(glfwGetWindowUserPointer(window));
		*p_resizeOccurred = true;
	}

	void Device::createWindow()
	{
		glfwInit();
		glfwWindowHint(
			GLFW_CLIENT_API,
			GLFW_NO_API
		); // Don't create OpenGL context
		glfwWindowHint(
			GLFW_RESIZABLE,
			true
		); // Temp: make window not resizable
		window = glfwCreateWindow(
			windowSize[0],
			windowSize[1],
			gameTitle.c_str(),
			nullptr,
			nullptr
		);
		glfwSetWindowUserPointer(window, &resizeOccurred);

		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	vk::SampleCountFlags Device::min(const vk::SampleCountFlags &a, const vk::SampleCountFlags &b)
	{
		auto cast_a = static_cast<uint32_t>(a);
		auto cast_b = static_cast<uint32_t>(b);

		if (cast_a < cast_b) {
			return a;
		}

		return b;
	}
}
