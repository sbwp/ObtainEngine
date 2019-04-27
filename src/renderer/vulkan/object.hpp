#ifndef OBTAIN_OBJECT_HPP
#define OBTAIN_OBJECT_HPP

#include <vector>

#include "vertex2d.hpp"

namespace Obtain::Graphics::Vulkan {
	class Object {
	public:
		Object();

		std::vector<Vertex2D> getVertices();
		std::vector<uint32_t> getIndices();

		uint32_t getBufferSize();
		uint32_t getIndexBufferSize();

	private:
		std::vector<Vertex2D> vertices;
		std::vector<uint32_t> indices;
	};
}

#endif // OBTAIN_OBJECT_HPP
