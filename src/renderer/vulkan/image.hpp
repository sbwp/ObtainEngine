//
// Created by sabrina on 5/2/19.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vulkan/vulkan.hpp>
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Image {
	public:
		Image(std::unique_ptr<Device> &device, vk::UniqueSampler &sampler, uint32_t width, uint32_t height,
		      vk::Format format, vk::ImageTiling tiling,
		      const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags);

		static Image createTextureImage(std::unique_ptr<Device> &device, vk::UniqueSampler &sampler,
		                                vk::UniqueCommandPool &pool, const vk::Queue &graphicsQueue,
		                                const std::string &file);

		void transitionLayout(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                      const vk::Format &format, const vk::ImageLayout &oldLayout,
		                      const vk::ImageLayout &newLayout);

		void copyFromBuffer(vk::UniqueCommandPool &commandPool, const vk::Queue &graphicsQueue,
		                    Buffer &buffer, uint32_t width, uint32_t height);
	private:
		std::unique_ptr<Device> &device;
		vk::UniqueImage image;
		vk::UniqueDeviceMemory memory;
		vk::UniqueImageView view;
		vk::UniqueSampler &sampler;

		static const vk::AccessFlags accessMaskForLayout(const vk::ImageLayout &layout);
		static const vk::PipelineStageFlags pipelineStageForLayout(const vk::ImageLayout &layout);
	};
}


#endif // IMAGE_HPP
