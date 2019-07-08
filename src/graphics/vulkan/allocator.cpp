//
// Created by sabrina on 5/11/19.
//

#include "allocator.hpp"
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	Allocator::Allocator(Device *device)
		: device(device)
	{}

	Memory Allocator::allocate(vk::DeviceSize size, vk::DeviceSize offset, uint32_t type)
	{
		auto memory = vk::UniqueDeviceMemory();
		return Memory{memory, size, offset, type};
	}


	/******************************************
	 ***************** private *****************
	 ******************************************/

}