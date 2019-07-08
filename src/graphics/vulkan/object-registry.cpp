//
// Created by sabrina on 5/2/19.
//

#include "object-registry.hpp"

namespace Obtain::Graphics::Vulkan {
	ObjectRegistry &ObjectRegistry::get()
	{
		static ObjectRegistry instance;
		return instance;
	}
}