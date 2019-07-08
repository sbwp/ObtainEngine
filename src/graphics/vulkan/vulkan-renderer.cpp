#include "vulkan-renderer.hpp"

#include <vector>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "device.hpp"
#include "queue-family-indices.hpp"
#include "command.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	VulkanRenderer::VulkanRenderer(
		const std::string &gameTitle,
		std::array<uint32_t, 3> gameVersion,
		std::array<uint32_t, 3> engineVersion
	)
		: swapchain(nullptr)
	{
		device = new Device(gameTitle,
		                    gameVersion,
		                    engineVersion);

		graphicsQueue = device->getGraphicsQueue();
		presentationQueue = device->getPresentQueue();

		commandPool = device->createCommandPool();

		obj = Object::unique(device, commandPool, "chalet.obj", "chalet.jpg");

		sampler = obj->getTextureImage()->createSampler();



		vertexBuffer = createAndLoadBuffer(static_cast<vk::DeviceSize>(obj->getVertexBufferSize()),
		                                   vk::BufferUsageFlagBits::eVertexBuffer, obj->getVertices().data());

		indexBuffer = createAndLoadBuffer(static_cast<vk::DeviceSize>(obj->getIndexBufferSize()),
		                                  vk::BufferUsageFlagBits::eIndexBuffer, obj->getIndices().data());

		swapchain = new Swapchain(
			device,
			device->getWindowSize(),
			indices,
			commandPool,
			vertexBuffer,
			indexBuffer,
			obj->getTextureImage(),
			sampler
		);

	}

	VulkanRenderer::~VulkanRenderer()
	{
		obj.reset();
		delete (swapchain);
		vertexBuffer.reset();
		indexBuffer.reset();
		commandPool.reset();
		sampler.reset();
		delete(device);
	}

	void VulkanRenderer::run()
	{
		while (device->windowOpen()) {
			glfwPollEvents();
			drawFrame();
			bool drawSuccess = swapchain->submitFrame(*graphicsQueue, *presentationQueue);
			if (device->getResizeFlag() || !drawSuccess) {
				updateWindowSize();
			}
		}

		device->waitIdle();
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/

	void VulkanRenderer::drawFrame()
	{

	}

	void VulkanRenderer::updateWindowSize()
	{
		device->updateWindowSizeOnceVisible();

		delete (swapchain);
		swapchain = new Swapchain(
			device,
			device->getWindowSize(),
			indices,
			commandPool,
			vertexBuffer,
			indexBuffer,
			obj->getTextureImage(),
			sampler
		);

		device->resetResizeFlag();
	}

	std::unique_ptr<Buffer> VulkanRenderer::createAndLoadBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
	                                                            void *data)
	{
		Buffer stagingBuffer = Buffer(
			device,
			size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		stagingBuffer.load(0u, data, static_cast<size_t>(size));

		std::unique_ptr<Buffer> newBuffer = std::make_unique<Buffer>(
			Buffer(
				device,
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
		vk::BufferCopy region(srcOffset, dstOffset, size);

		auto action = [
			&src,
			&dst,
			&region
		](vk::CommandBuffer commandBuffer) {
			commandBuffer.copyBuffer(
				*src,
				*dst,
				1u,
				&region
			);
		};

		Command::runSingleTime(device, commandPool, *graphicsQueue, action);
	}
}
