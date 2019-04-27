#include "vulkan-renderer.hpp"

#include <vector>
#include <iostream>

#include "instance.hpp"
#include "validation.hpp"
#include "device.hpp"
#include "queue-family-indices.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	VulkanRenderer::VulkanRenderer(
		std::string gameTitle,
		std::array<uint32_t, 3> gameVersion,
		std::array<uint32_t, 3> engineVersion
	)
		: gameVersion(gameVersion),
		  gameTitle(gameTitle),
		  resizeOccurred(false)
	{

		windowSize = {1600, 900};
		initWindow();

		instance = Instance::createVkInstance(
			gameTitle,
			gameVersion,
			engineVersion
		);
		loader.init(*instance);
		debugMessenger = Validation::createDebugMessenger(
			instance,
			loader
		);

		surface = Device::createSurface(
			*instance,
			window
		);
		physicalDevice = Device::selectPhysicalDevice(
			instance,
			surface
		);
		device = Device::createLogicalDevice(
			instance,
			physicalDevice,
			surface
		);

		indices = QueueFamilyIndices::findQueueFamilies(
			*physicalDevice,
			surface
		);
		graphicsQueue = device->getQueue(
			indices.graphicsFamily.value(),
			0
		);
		presentationQueue = device->getQueue(
			indices.presentFamily.value(),
			0
		);

		createCommandPool();

		vertexBuffer = createAndLoadBuffer(static_cast<vk::DeviceSize>(obj.getBufferSize()),
		                                   vk::BufferUsageFlagBits::eVertexBuffer, obj.getVertices().data());

		indexBuffer = createAndLoadBuffer(static_cast<vk::DeviceSize>(obj.getIndexBufferSize()),
		                                   vk::BufferUsageFlagBits::eIndexBuffer, obj.getIndices().data());

		swapchain = new Swapchain(
			instance,
			physicalDevice,
			device,
			surface,
			windowSize,
			indices,
			commandPool,
			vertexBuffer,
			indexBuffer
		);

	}

	VulkanRenderer::~VulkanRenderer()
	{
		delete (swapchain);
		instance->destroyDebugUtilsMessengerEXT(
			debugMessenger,
			nullptr,
			loader
		);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VulkanRenderer::run()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			drawFrame();
			bool drawSuccess = swapchain->submitFrame(graphicsQueue, presentationQueue);
			if (resizeOccurred || !drawSuccess) {
				updateWindowSize();
			}
		}

		device->waitIdle();
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	void framebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto p_resizeOccurred = reinterpret_cast<bool *>(glfwGetWindowUserPointer(window));
		*p_resizeOccurred = true;
	}

	void VulkanRenderer::initWindow()
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

	void VulkanRenderer::drawFrame()
	{

	}

	void VulkanRenderer::createCommandPool()
	{
		commandPool = device->createCommandPoolUnique(
			vk::CommandPoolCreateInfo(
				vk::CommandPoolCreateFlags(),
				QueueFamilyIndices::findQueueFamilies(
					*physicalDevice,
					surface
				).graphicsFamily.value()
			)
		);
	}

	void VulkanRenderer::updateWindowSize()
	{
		int width = 0, height = 0;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
		windowSize = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		delete (swapchain);
		swapchain = new Swapchain(
			instance,
			physicalDevice,
			device,
			surface,
			windowSize,
			indices,
			commandPool,
			vertexBuffer,
			indexBuffer
		);
		resizeOccurred = false;
	}

	std::unique_ptr<Buffer> VulkanRenderer::createAndLoadBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, void *data)
	{
		Buffer stagingBuffer = Buffer(
			device,
			physicalDevice,
			size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		stagingBuffer.load(0u, data, static_cast<size_t>(size));

		std::unique_ptr<Buffer> newBuffer = std::make_unique<Buffer>(
			Buffer(
				device,
				physicalDevice,
				size,
				vk::BufferUsageFlagBits::eTransferDst | usageFlags,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			)
		);

		copyBuffer(
			stagingBuffer.getBuffer(), 0u,
			newBuffer->getBuffer(), 0u,
			size
		);

		return newBuffer;
	}

	void VulkanRenderer::copyBuffer(
		vk::UniqueBuffer &src, vk::DeviceSize srcOffset, vk::UniqueBuffer &dst, vk::DeviceSize dstOffset,
		vk::DeviceSize size
	)
	{
		std::vector<vk::UniqueCommandBuffer> allocatedCommandBuffers = device->allocateCommandBuffersUnique(
			vk::CommandBufferAllocateInfo(
				*commandPool,
				vk::CommandBufferLevel::ePrimary,
				1u
			)
		);

		allocatedCommandBuffers[0]->begin(
			vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
		);

		vk::BufferCopy region(srcOffset, dstOffset, size);

		allocatedCommandBuffers[0]->copyBuffer(
			*src,
			*dst,
			1u,
			&region
		);

		allocatedCommandBuffers[0]->end();

		vk::UniqueFence fence = device->createFenceUnique(vk::FenceCreateInfo());

		vk::SubmitInfo submitInfo(
			0u,
			nullptr,
			nullptr,
			1u,
			&allocatedCommandBuffers[0].get()
		);
		graphicsQueue.submit(1, &submitInfo, *fence);
		device->waitForFences(
			1u, &fence.get(), true, std::numeric_limits<uint64_t>::max()
		);
	}
}
