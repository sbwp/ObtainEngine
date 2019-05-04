//
// Created by austin on 5/2/19.
//

#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "buffer.hpp"
#include "device.hpp"
#include "command.hpp"

#define TEXTURE_LOCATION "assets/textures/"

namespace Obtain::Graphics::Vulkan {
	Image::Image(std::unique_ptr<Device> &device, vk::UniqueSampler &sampler,
	             uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
	             const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags)
	             : device(device), sampler(sampler)
	{
		vk::Extent3D extent(width, height, 1u);

		image = device->createImage(extent, format, tiling, usageFlags);

		auto memoryRequirements = device->getImageMemoryRequirements(image);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);
		memory = device->allocateMemory(memoryRequirements.size, memoryType);
		device->bindImageMemory(image, memory, 0);
		view = device->createImageView(image, format);
	}

	Image Image::createTextureImage(std::unique_ptr<Device> &device, vk::UniqueSampler &sampler,
	                                vk::UniqueCommandPool &pool, const vk::Queue &graphicsQueue,
	                                const std::string &file)
	{
		std::string filePath = TEXTURE_LOCATION + file;
		int width, height, channels;
		stbi_uc *pixels = stbi_load(filePath.c_str(),
		                            &width,
		                            &height,
		                            &channels,
		                            STBI_rgb_alpha);

		if (!pixels) {
			throw std::runtime_error("failed to load texture from file " + filePath);
		}

		vk::DeviceSize size = width * height * 4;

		auto stagingBuffer = Buffer(device,
		                            size,
		                            vk::BufferUsageFlagBits::eTransferSrc,
		                            vk::MemoryPropertyFlagBits::eHostCoherent |
		                            vk::MemoryPropertyFlagBits::eHostVisible);

		stagingBuffer.load(0, pixels, size);

		stbi_image_free(pixels);

		auto image = Image(device,
		             sampler,
		             width,
		             height,
		             vk::Format::eR8G8B8A8Unorm,
		             vk::ImageTiling::eOptimal,
		             vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		             vk::MemoryPropertyFlagBits::eDeviceLocal);

		image.transitionLayout(pool, graphicsQueue, vk::Format::eR8G8B8A8Unorm,
		                       vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		image.copyFromBuffer(pool, graphicsQueue, stagingBuffer,
		                     static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		image.transitionLayout(pool, graphicsQueue, vk::Format::eR8G8B8A8Unorm,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		return image;
	}

	void Image::transitionLayout(
		vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::Format &format,
		const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout)
	{

		auto action = [&oldLayout, &newLayout, this](vk::CommandBuffer commandBuffer) {
			vk::ImageMemoryBarrier barrier(accessMaskForLayout(oldLayout),
			                               accessMaskForLayout(newLayout),
			                               oldLayout,
			                               newLayout,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               *image,
			                               vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
			                                                         0u,
			                                                         1u,
			                                                         0u,
			                                                         1u));

			commandBuffer.pipelineBarrier(pipelineStageForLayout(oldLayout),
			                              pipelineStageForLayout(newLayout),
			                               vk::DependencyFlags(),
			                               0u, nullptr,
			                               0u, nullptr,
			                               1u, &barrier);
		};

		Command::runSingleTime(device, commandPool, graphicsQueue, action);
	}

	void Image::copyFromBuffer(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
	                           Buffer &buffer, uint32_t width, uint32_t height)
	{

		vk::ImageSubresourceLayers subresource(vk::ImageAspectFlagBits::eColor,
		                                       0, 0, 1);
		vk::BufferImageCopy region(0, 0, 0,
		                           subresource, vk::Offset3D(0, 0, 0),
		                           vk::Extent3D(width, height, 1));

		buffer.copyToImage(commandPool, graphicsQueue, image,
		                   region, subresource);
	}


	const vk::AccessFlags Image::accessMaskForLayout(const vk::ImageLayout &layout)
	{
		switch(layout) {
			case vk::ImageLayout::eUndefined: return vk::AccessFlags();
			case vk::ImageLayout::eTransferDstOptimal: return vk::AccessFlagBits::eTransferWrite;
			case vk::ImageLayout::eShaderReadOnlyOptimal: return vk::AccessFlagBits::eShaderRead;
			default: throw std::invalid_argument("unsupported image layout");
		}
	}

	const vk::PipelineStageFlags Image::pipelineStageForLayout(const vk::ImageLayout &layout)
	{
		switch(layout) {
			case vk::ImageLayout::eUndefined: return vk::PipelineStageFlagBits::eTopOfPipe;
			case vk::ImageLayout::eTransferDstOptimal: return vk::PipelineStageFlagBits::eTransfer;
			case vk::ImageLayout::eShaderReadOnlyOptimal: return vk::PipelineStageFlagBits::eFragmentShader;
			default: throw std::invalid_argument("unsupported image layout");
		}
	}

}
