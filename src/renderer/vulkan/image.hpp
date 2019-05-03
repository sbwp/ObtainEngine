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
		Image(std::unique_ptr<Device> &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice,
		      uint32_t width, uint32_t height,
		      vk::Format format, vk::ImageTiling tiling,
		      const vk::ImageUsageFlags &usageFlags, const vk::MemoryPropertyFlags &propertyFlags);
		static Image createTextureImage(std::unique_ptr<Device> &device, std::unique_ptr<vk::PhysicalDevice> &physicalDevice, const std::string &file);
		static void transitionImageLayout(std::unique_ptr<Device> &device, vk::UniqueCommandPool &commandPool,
		                                  const vk::Queue &graphicsQueue, const vk::Image &image, const vk::Format &format,
		                                  const vk::ImageLayout &oldLayout, const vk::ImageLayout &newLayout);
		static void copyBufferToImage(std::unique_ptr<Device> &device, vk::UniqueCommandPool &commandPool,
		                             const vk::Queue &graphicsQueue, const vk::Buffer &buffer, const vk::Image &image,
		                             uint32_t width, uint32_t height);
	private:
		vk::UniqueImage image;
		vk::UniqueDeviceMemory memory;

	};
}


#endif // IMAGE_HPP
