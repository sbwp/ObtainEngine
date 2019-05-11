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
	Image::Image(Device *device, uint32_t width, uint32_t height, uint32_t mipLevels,
	             vk::Format format, vk::ImageTiling tiling, const vk::ImageAspectFlags &aspectMask,
	             const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags,
	             vk::SampleCountFlagBits sampleCount)
		: device(device), format(format), mipLevels(mipLevels), extent(width, height, 1u)
	{
		image = device->createImage(extent, format, mipLevels, tiling,
		                            usageFlags, sampleCount);

		auto memoryRequirements = device->getImageMemoryRequirements(image);
		auto memoryType = device->findMemoryType(memoryRequirements.memoryTypeBits, propertyFlags);
		memory = device->allocateMemory(memoryRequirements.size, memoryType);
		device->bindImageMemory(image, memory, 0);
		view = device->createImageView(image, format, mipLevels, aspectMask);
	}

	std::unique_ptr<Image> Image::unique(Device *device, uint32_t width, uint32_t height, uint32_t mipLevels,
	                                     vk::Format format, vk::ImageTiling tiling,
	                                     const vk::ImageAspectFlags &aspectMask,
	                                     const vk::ImageUsageFlags &usageFlags,
	                                     const vk::MemoryPropertyFlags &propertyFlags,
	                                     vk::SampleCountFlagBits sampleCount)
	{
		return std::make_unique<Image>(Image(device, width, height, mipLevels,
		                                     format, tiling, aspectMask, usageFlags,
		                                     propertyFlags, sampleCount));
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

		uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))) + 1);

		auto image = Image::unique(device,
		                           width,
		                           height,
		                           mipLevels,
		                           vk::Format::eR8G8B8A8Unorm,
		                           vk::ImageTiling::eOptimal,
		                           vk::ImageAspectFlagBits::eColor,
		                           vk::ImageUsageFlagBits::eTransferSrc |
		                           vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		                           vk::MemoryPropertyFlagBits::eDeviceLocal);

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

		image->copyFromBuffer(pool, *device->getGraphicsQueue(), stagingBuffer,
		                      static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		image->generateMipmaps(pool);

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
		                           width, height, 1U,
		                           format,
		                           vk::ImageTiling::eOptimal,
		                           vk::ImageAspectFlagBits::eDepth,
		                           vk::ImageUsageFlagBits::eDepthStencilAttachment,
		                           vk::MemoryPropertyFlagBits::eDeviceLocal,
		                           device->getSampleCount());

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                        vk::ImageLayout::eUndefined,
		                        vk::ImageLayout::eDepthStencilAttachmentOptimal);
		return image;
	}

	std::unique_ptr<Image> Image::createColorImage(Device *device, const vk::Extent2D &extent, const vk::Format &format,
	                                        vk::UniqueCommandPool &pool)
	{
		auto image = Image::unique(device,
		                           extent.width, extent.height, 1U,
		                           format,
		                           vk::ImageTiling::eOptimal,
		                           vk::ImageAspectFlagBits::eColor,
		                           vk::ImageUsageFlagBits::eTransientAttachment |
		                           vk::ImageUsageFlagBits::eColorAttachment,
		                           vk::MemoryPropertyFlagBits::eDeviceLocal,
		                           device->getSampleCount());

		image->transitionLayout(pool, *device->getGraphicsQueue(),
		                        vk::ImageLayout::eUndefined,
		                        vk::ImageLayout::eColorAttachmentOptimal);
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
				                               mipLevels,
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

	vk::UniqueSampler Image::createSampler()
	{
		return device->createSampler(static_cast<float>(mipLevels));
	}

	vk::Format &Image::getFormat()
	{
		return format;
	}

	/****************************************************
	 ******************* Private ************************
	 ****************************************************/

	void Image::generateMipmaps(vk::UniqueCommandPool &pool)
	{
		if (!device->hasOptimalTilingFeature(format,
		                                     vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
			throw std::runtime_error("Failed to generate mipmaps: image format does not support linear filter");
		}
		vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor,
		                                           0, 1, 0, 1);

		vk::ImageMemoryBarrier barrier(vk::AccessFlags(),
		                               vk::AccessFlags(),
		                               vk::ImageLayout::eUndefined, vk::ImageLayout::eUndefined,
		                               (VK_QUEUE_FAMILY_IGNORED),
		                               (VK_QUEUE_FAMILY_IGNORED),
		                               *image, subresourceRange);


		auto action = [&barrier,
		               this](vk::CommandBuffer commandBuffer) {
			int32_t width = extent.width;
			int32_t height = extent.height;

			for (uint32_t i = 1; i < mipLevels; i++) {
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
				barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
				barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
				barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
				commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
				                              vk::PipelineStageFlagBits::eTransfer,
				                              vk::DependencyFlags(), 0,
				                              nullptr, 0,
				                              nullptr, 1,
				                              &barrier);

				vk::ImageSubresourceLayers srcSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				                                                i - 1, 0, 1);
				vk::ImageSubresourceLayers dstSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				                                                i, 0, 1);
				vk::Offset3D sharedOffset(0, 0, 0);
				vk::Offset3D srcOffset(width, height, 1);
				vk::Offset3D dstOffset(width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1);
				vk::ImageBlit blit(srcSubresourceLayers, {sharedOffset, srcOffset},
				                   dstSubresourceLayers, {sharedOffset, dstOffset});
				commandBuffer.blitImage(*image, vk::ImageLayout::eTransferSrcOptimal,
				                        *image, vk::ImageLayout::eTransferDstOptimal,
				                        1, &blit, vk::Filter::eLinear);

				barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
				barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
				commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
				                              vk::PipelineStageFlagBits::eFragmentShader,
				                              vk::DependencyFlags(), 0,
				                              nullptr, 0,
				                              nullptr, 1,
				                              &barrier);

				if (width > 1) {
					width /= 2;
				}

				if (height > 1) {
					height /= 2;
				}
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
			                              vk::PipelineStageFlagBits::eFragmentShader,
			                              vk::DependencyFlags(), 0,
			                              nullptr, 0,
			                              nullptr, 1,
			                              &barrier);
		};

		Command::runSingleTime(device, pool, *device->getGraphicsQueue(), action);
	}

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
			case vk::ImageLayout::eColorAttachmentOptimal:
				return vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
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
			case vk::ImageLayout::eColorAttachmentOptimal:
				return vk::PipelineStageFlagBits::eColorAttachmentOutput;
			default:
				throw std::invalid_argument("unsupported image layout");
		}
	}

	vk::ImageAspectFlags Image::aspectMaskForLayout(const vk::ImageLayout &layout)
	{
		 if (layout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			 vk::ImageAspectFlags mask = vk::ImageAspectFlagBits::eDepth;
			 if (hasStencilComponent()) {
				 mask |= vk::ImageAspectFlagBits::eStencil;
			 }
			 return mask;
		 }

		return vk::ImageAspectFlagBits::eColor;
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
