//
// Created by austin on 5/2/19.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vulkan/vulkan.hpp>
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Image {
	public:
		Image(Device *device, uint32_t width, uint32_t height, uint32_t mipLevels,
		      vk::Format format, vk::ImageTiling tiling, const vk::ImageAspectFlags &aspectMask,
		      const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags,
		      vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1);

		static std::unique_ptr<Image> unique(Device *device, uint32_t width, uint32_t height, uint32_t mipLevels,
		                                     vk::Format format, vk::ImageTiling tiling,
		                                     const vk::ImageAspectFlags &aspectMask,
		                                     const vk::ImageUsageFlags &usageFlags,
		                                     const vk::MemoryPropertyFlags &propertyFlags,
		                                     vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1);

		static std::unique_ptr<Image> createTextureImage(Device *device, vk::UniqueCommandPool &pool,
		                                                 const std::string &file);

		static std::unique_ptr<Image> createDepthImage(Device *device, const vk::Extent2D &extent,
		                                               vk::UniqueCommandPool &pool);

		static std::unique_ptr<Image> createColorImage(Device *device, const vk::Extent2D &extent,
		                                               const vk::Format &format,
		                                               vk::UniqueCommandPool &pool);

		vk::UniqueImageView &getView();

		void transitionLayout(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                      const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout);

		void copyFromBuffer(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                    Buffer &buffer, uint32_t width, uint32_t height);

		bool hasStencilComponent();

		vk::UniqueSampler createSampler();

		vk::Format &getFormat();
	private:
		Device *device;
		vk::UniqueImage image;
		vk::UniqueDeviceMemory memory;
		vk::UniqueImageView view;
		vk::Format format;
		vk::Extent3D extent;

		uint32_t mipLevels;

		vk::ImageAspectFlags aspectMaskForLayout(const vk::ImageLayout &layout);
		void generateMipmaps(vk::UniqueCommandPool &pool);

		static const vk::AccessFlags accessMaskForLayout(const vk::ImageLayout &layout);
		static const vk::PipelineStageFlags pipelineStageForLayout(const vk::ImageLayout &layout);

		static const vk::Format findSupportedFormat(Device *device,
		                                            const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
		                                            vk::FormatFeatureFlags features);
	};
}


#endif // IMAGE_HPP
