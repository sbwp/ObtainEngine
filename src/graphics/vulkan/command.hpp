//
// Created by sabrina on 5/2/19.
//

#ifndef OBTAIN_GRAPHICS_VULKAN_COMMAND_HPP
#define OBTAIN_GRAPHICS_VULKAN_COMMAND_HPP

#include <vulkan/vulkan.hpp>
#include <functional>

#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	class Command {
	public:
		static void runSingleTime(Device *device, vk::UniqueCommandPool &pool,
		                          const vk::Queue &graphicsQueue, std::function<void(vk::CommandBuffer)> action);
	};
}


#endif // OBTAIN_GRAPHICS_VULKAN_COMMAND_HPP
