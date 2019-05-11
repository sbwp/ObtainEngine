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
		Image(Device *device, uint32_t width, uint32_t height,
		      vk::Format format, vk::ImageTiling tiling, const vk::ImageAspectFlags &aspectMask,
		      const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags);

		static std::unique_ptr<Image> unique(Device *device, uint32_t width, uint32_t height,
		                                     vk::Format format, vk::ImageTiling tiling,
		                                     const vk::ImageAspectFlags &aspectMask,
		                                     const vk::ImageUsageFlags &usageFlags,
		                                     const vk::MemoryPropertyFlags &propertyFlags);

		static std::unique_ptr<Image> createTextureImage(Device *device, vk::UniqueCommandPool &pool,
		                                                 const std::string &file);

		static std::unique_ptr<Image> createDepthImage(Device *device, const vk::Extent2D &extent,
		                                               vk::UniqueCommandPool &pool);

		vk::UniqueImageView &getView();

		void transitionLayout(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                      const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout);

		void copyFromBuffer(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                    Buffer &buffer, uint32_t width, uint32_t height);

		bool hasStencilComponent();

		vk::Format &getFormat();
	private:
		Device *device;
		vk::UniqueImage image;
		vk::UniqueDeviceMemory memory;
		vk::UniqueImageView view;
		vk::Format format;

		static const vk::AccessFlags accessMaskForLayout(const vk::ImageLayout &layout);
		static const vk::PipelineStageFlags pipelineStageForLayout(const vk::ImageLayout &layout);
		const vk::ImageAspectFlags aspectMaskForLayout(const vk::ImageLayout &layout);

		static const vk::Format findSupportedFormat(Device *device,
		                                            const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
		                                            vk::FormatFeatureFlags features);
	};
}


#endif // IMAGE_HPP
