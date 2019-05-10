//
// Created by austin on 5/2/19.
//

#ifndef COMMAND_HPP
#define COMMAND_HPP

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


#endif // COMMAND_HPP
