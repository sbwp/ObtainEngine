//
// Singleton that handles the list of currently rendered objects
// Created by sabrina on 5/2/19.
//

#ifndef OBJECT_REGISTRY_HPP
#define OBJECT_REGISTRY_HPP

#include <set>

#include "object.hpp"

namespace Obtain::Graphics::Vulkan {
	class ObjectRegistry {
	public:
		ObjectRegistry &get();
		ObjectRegistry(const ObjectRegistry &) = delete;
		ObjectRegistry& operator = (const ObjectRegistry &) = delete;
	private:
		ObjectRegistry() = default;
		~ObjectRegistry() = default;

		std::set<Object> objects;

	};
}

#endif // OBJECT_REGISTRY_HPP
