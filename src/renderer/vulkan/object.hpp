#ifndef OBTAIN_OBJECT_HPP
#define OBTAIN_OBJECT_HPP

#include <vector>

#include "vertex2d.hpp"

namespace Obtain::Graphics::Vulkan {
	class Object {
	public:
		Object();

		std::vector<Vertex2D> getVertices();

		uint32_t getBufferSize();

	private:
		std::vector<Vertex2D> vertices;
	};
}

#endif // OBTAIN_OBJECT_HPP
