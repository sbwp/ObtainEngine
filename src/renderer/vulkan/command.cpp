//
// Created by austin on 5/2/19.
//

#include "command.hpp"

namespace Obtain::Graphics::Vulkan {
	void Command::runSingleTime(Device *device, vk::UniqueCommandPool &pool,
	                            const vk::Queue &graphicsQueue, std::function<void(vk::CommandBuffer)> action)
	{
		std::vector<vk::UniqueCommandBuffer> allocatedCommandBuffers =
			device->allocateCommandBuffers(pool,
			                               vk::CommandBufferLevel::ePrimary, 1u);

		allocatedCommandBuffers[0]->begin(
			vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		action(*allocatedCommandBuffers[0]);

		allocatedCommandBuffers[0]->end();

		vk::UniqueFence fence = device->createFence();

		vk::SubmitInfo submitInfo(
			0u,
			nullptr,
			nullptr,
			1u,
			&allocatedCommandBuffers[0].get()
		);
		graphicsQueue.submit(1, &submitInfo, *fence);
		device->waitForFence(fence);
	}
}
