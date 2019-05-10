//
// Created by sabrina on 5/2/19.
//

#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "buffer.hpp"
#include "device.hpp"
#include "command.hpp"

#define TEXTURE_LOCATION "assets/textures/"

namespace Obtain::Graphics::Vulkan {
	Image::Image(Device *device, uint32_t width, uint32_t height,
	             vk::Format format, vk::ImageTiling tiling, const vk::ImageAspectFlags &aspectMask,
	             const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags)
		: device(device), format(format)
	{
		vk::Extent3D extent(width, height, 1u);

		image = device->createImage(extent, format, tiling, usageFlags);

		auto memoryRequirements = device->getImageMemoryRequirements(image);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);
		memory = device->allocateMemory(memoryRequirements.size, memoryType);
		device->bindImageMemory(image, memory, 0);
		view = device->createImageView(image, format, aspectMask);
	}

	std::unique_ptr<Image> Image::unique(Device *device, uint32_t width, uint32_t height,
	                                     vk::Format format, vk::ImageTiling tiling,
	                                     const vk::ImageAspectFlags &aspectMask,
	                                     const vk::ImageUsageFlags &usageFlags,
	                                     const vk::MemoryPropertyFlags &propertyFlags)
	{
		return std::make_unique<Image>(Image(device, width, height, format, tiling,
		                              aspectMask, usageFlags, propertyFlags));
	}

	std::unique_ptr<Image> Image::createTextureImage(Device *device, vk::UniqueCommandPool &pool,
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

		auto image = Image::unique(device,
		                   width,
		                   height,
		                   vk::Format::eR8G8B8A8Unorm,
		                   vk::ImageTiling::eOptimal,
		                   vk::ImageAspectFlagBits::eColor,
		                   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		                   vk::MemoryPropertyFlagBits::eDeviceLocal);

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                       vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		image->copyFromBuffer(pool, *device->getGraphicsQueue(), stagingBuffer,
		                     static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                       vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		return image;
	}

	std::unique_ptr<Image> Image::createDepthImage(Device *device, const vk::Extent2D &extent,
	                              vk::UniqueCommandPool &pool)
	{
		vk::DeviceSize width = extent.width, height = extent.height;
		auto format = findSupportedFormat(device,
		                                  {
			                                  vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
			                                  vk::Format::eD24UnormS8Uint
		                                  },
		                                  vk::ImageTiling::eOptimal,
		                                  vk::FormatFeatureFlagBits::eDepthStencilAttachment);

		auto image = Image::unique(device,
		                   width, height,
		                   format,
		                   vk::ImageTiling::eOptimal,
		                   vk::ImageAspectFlagBits::eDepth,
		                   vk::ImageUsageFlagBits::eDepthStencilAttachment,
		                   vk::MemoryPropertyFlagBits::eDeviceLocal);

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                       vk::ImageLayout::eUndefined,
		                       vk::ImageLayout::eDepthStencilAttachmentOptimal);
		return image;
	}

	vk::UniqueImageView &Image::getView()
	{
		return view;
	}

	void Image::transitionLayout(
		vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout)
	{

		auto action = [&oldLayout,
		               &newLayout,
		               this](vk::CommandBuffer commandBuffer) {
			vk::ImageMemoryBarrier barrier(accessMaskForLayout(oldLayout),
			                               accessMaskForLayout(newLayout),
			                               oldLayout,
			                               newLayout,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               VK_QUEUE_FAMILY_IGNORED,
			                               *image,
			                               vk::ImageSubresourceRange(
				                               aspectMaskForLayout(newLayout),
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

	bool Image::hasStencilComponent()
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

	vk::Format &Image::getFormat()
	{
		return format;
	}

	/****************************************************
	 ******************* Private ************************
	 ****************************************************/

	const vk::AccessFlags Image::accessMaskForLayout(const vk::ImageLayout &layout)
	{
		switch (layout) {
			case vk::ImageLayout::eUndefined:
				return vk::AccessFlags();
			case vk::ImageLayout::eTransferDstOptimal:
				return vk::AccessFlagBits::eTransferWrite;
			case vk::ImageLayout::eShaderReadOnlyOptimal:
				return vk::AccessFlagBits::eShaderRead;
			case vk::ImageLayout::eDepthStencilAttachmentOptimal:
				return vk::AccessFlagBits::eDepthStencilAttachmentRead |
				       vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			default:
				throw std::invalid_argument("unsupported image layout");
		}
	}

	const vk::PipelineStageFlags Image::pipelineStageForLayout(const vk::ImageLayout &layout)
	{
		switch (layout) {
			case vk::ImageLayout::eUndefined:
				return vk::PipelineStageFlagBits::eTopOfPipe;
			case vk::ImageLayout::eTransferDstOptimal:
				return vk::PipelineStageFlagBits::eTransfer;
			case vk::ImageLayout::eShaderReadOnlyOptimal:
				return vk::PipelineStageFlagBits::eFragmentShader;
			case vk::ImageLayout::eDepthStencilAttachmentOptimal:
				return vk::PipelineStageFlagBits::eEarlyFragmentTests;
			default:
				throw std::invalid_argument("unsupported image layout");
		}
	}

	const vk::ImageAspectFlags Image::aspectMaskForLayout(const vk::ImageLayout &layout)
	{
		switch (layout) {
			case vk::ImageLayout::eDepthStencilAttachmentOptimal:
				return vk::ImageAspectFlagBits::eDepth;
			default:
				return vk::ImageAspectFlagBits::eColor;
		}
	}

	const vk::Format Image::findSupportedFormat(Device *device,
	                                            const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
	                                            vk::FormatFeatureFlags features)
	{
		for (vk::Format format : candidates) {
			auto properties = device->getFormatProperties(format);

			vk::FormatFeatureFlags desired;
			switch (tiling) {
				case vk::ImageTiling::eLinear:
					desired = properties.linearTilingFeatures;
					break;
				case vk::ImageTiling::eOptimal:
					desired = properties.optimalTilingFeatures;
					break;
				default:
					continue;
			}

			if ((desired & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format");
	}

}
