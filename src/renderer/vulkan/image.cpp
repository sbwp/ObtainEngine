//
// Created by austin on 5/2/19.
//

#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "buffer.hpp"
#include "device.hpp"
#include "command.hpp"

namespace Obtain::Graphics::Vulkan {
	Image::Image(std::unique_ptr<Device> &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice,
	             uint32_t width, uint32_t height,
	             vk::Format format, vk::ImageTiling tiling,
	             const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags
	)
	{
		vk::Extent3D extent(width, height, 1u);

		image = device->createImage(extent, format, tiling, usageFlags);

		auto memoryRequirements = device->getImageMemoryRequirements(image);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);
		memory = device->allocateMemory(memoryRequirements.size, memoryType);
		device->bindImageMemory(image, memory, 0);
	}

	Image Image::createTextureImage(std::unique_ptr<Device> &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice, const std::string &file)
	{
		int width, height, channels;
		stbi_uc *pixels = stbi_load(file.c_str(),
		                            &width,
		                            &height,
		                            &channels,
		                            STBI_rgb_alpha);

		vk::DeviceSize size = width * height * 4;

		auto stagingBuffer = Buffer(device,
		                            size,
		                            vk::BufferUsageFlagBits::eTransferSrc,
		                            vk::MemoryPropertyFlagBits::eHostCoherent |
		                            vk::MemoryPropertyFlagBits::eHostVisible);

		stagingBuffer.load(0, pixels, size);

		stbi_image_free(pixels);

		return Image(device,
		             physicalDevice,
		             width,
		             height,
		             vk::Format::eR8G8B8A8Unorm,
		             vk::ImageTiling::eOptimal,
		             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		             vk::MemoryPropertyFlagBits::eDeviceLocal);
	}

	void Image::transitionImageLayout(std::unique_ptr<Device> &device, vk::UniqueCommandPool &commandPool,
	                                  const vk::Queue &graphicsQueue, const vk::Image &image, const vk::Format &format,
	                                  const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout)
	{

		auto action = [&oldLayout, &newLayout, &image](vk::CommandBuffer commandBuffer) {
			vk::ImageMemoryBarrier barrier(vk::AccessFlags(),
			                               vk::AccessFlags(),
			                               oldLayout,
			                               newLayout,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               image,
			                               vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
			                                                         0u,
			                                                         1u,
			                                                         0u,
			                                                         1u));

			commandBuffer.pipelineBarrier(vk::PipelineStageFlags(), vk::PipelineStageFlags(), // TODO
			                               vk::DependencyFlags(),
			                               0u, nullptr,
			                               0u, nullptr,
			                               1u, &barrier);
		};

		Command::runSingleTime(device, commandPool, graphicsQueue, action);
	}

	void Image::copyBufferToImage(std::unique_ptr<Device> &device, vk::UniqueCommandPool &commandPool,
	                              const vk::Queue &graphicsQueue, const vk::Buffer &buffer, const vk::Image &image,
	                              uint32_t width, uint32_t height)
	{

		vk::ImageSubresourceLayers subresource(vk::ImageAspectFlagBits::eColor,
		                                       0, 0, 1);
		vk::BufferImageCopy region(0, 0, 0,
		                           subresource, vk::Offset3D(0, 0, 0),
		                           vk::Extent3D(width, height, 1));

		auto action = [&buffer, &image, &region](vk::CommandBuffer commandBuffer){
			commandBuffer.copyBufferToImage(buffer, image,
				vk::ImageLayout::eTransferDstOptimal, 1, &region);
		};
		Command::runSingleTime(device, commandPool, graphicsQueue, action);
	}

}
