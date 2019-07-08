//
// Created by sabrina on 4/22/19.
//

#include "buffer.hpp"
#include "command.hpp"

namespace Obtain::Graphics::Vulkan {
	Buffer::Buffer(Device *device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
	               const vk::MemoryPropertyFlags &propertyFlags)
		: device(device), size(size)
	{
		buffer = device->createBuffer(size, usageFlags);

		auto memoryRequirements = device->getBufferMemoryRequirements(buffer);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits,
		                                         propertyFlags);

		memory = device->allocateMemory(memoryRequirements.size,
		                                memoryType);

		offset = 0u;

		device->bindBufferMemory(buffer, memory, offset);
	}

	std::unique_ptr<Buffer> Buffer::unique(Device *device, vk::DeviceSize size, const vk::BufferUsageFlags &usageFlags,
	                                       const vk::MemoryPropertyFlags &propertyFlags)
	{
		return std::make_unique<Buffer>(Buffer(device, size, usageFlags,
		                                       propertyFlags));
	}

	void Buffer::load(vk::DeviceSize internalOffset, void *source, vk::DeviceSize size)
	{
		device->setMemory(memory, offset + internalOffset, size, source);
	}

	vk::UniqueBuffer &Buffer::getBuffer()
	{
		return buffer;
	}

	vk::UniqueDeviceMemory &Buffer::getDeviceMemory()
	{
		return memory;
	}

	vk::DeviceSize Buffer::getSize()
	{
		return size;
	}

	vk::DeviceSize Buffer::getOffset()
	{
		return offset;
	}

	void Buffer::copyToImage(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue, vk::UniqueImage &image,
	                         const vk::BufferImageCopy &region, const vk::ImageSubresourceLayers &subresource)
	{
		auto action = [&image,
		               this,
		               &region](vk::CommandBuffer commandBuffer) {
			commandBuffer.copyBufferToImage(*buffer, *image,
			                                vk::ImageLayout::eTransferDstOptimal, 1, &region);
		};
		Command::runSingleTime(device, commandPool, graphicsQueue, action);
	}

	void Buffer::copyToBuffer(vk::UniqueCommandPool &pool, vk::Queue *queue, std::unique_ptr<Buffer> &dst)
	{
		vk::DeviceSize copySize = std::min(size, dst->size);
		vk::BufferCopy region(offset, dst->offset, copySize);

		auto action = [
			this,
			&dst,
			&region
		](vk::CommandBuffer commandBuffer) {
			commandBuffer.copyBuffer(
				*buffer,
				*(dst->buffer),
				1u,
				&region
			);
		};

		Command::runSingleTime(device, pool, *queue, action);
	}
}