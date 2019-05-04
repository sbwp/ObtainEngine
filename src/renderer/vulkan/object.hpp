#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "vertex.hpp"

namespace Obtain::Graphics::Vulkan {
	class Object {
	public:
		Object();

		std::vector<Vertex> getVertices();
		std::vector<uint32_t> getIndices();

		uint32_t getBufferSize();
		uint32_t getIndexBufferSize();

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
}

#endif // OBJECT_HPP
