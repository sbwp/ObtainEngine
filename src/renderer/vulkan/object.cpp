//
// Created by sabrina on 4/19/19.
//

#include "object.hpp"
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {

	Object::Object()
	{
		vertices = {
			{{-0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
			{{0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
		};

		indices = {
			0, 1, 2, 2, 3, 0
		};
	}

	std::vector<Vertex2D> Object::getVertices()
	{
		return vertices;
	}

	std::vector<uint32_t> Object::getIndices()
	{
		return indices;
	}

	uint32_t Object::getBufferSize()
	{
		return sizeof(vertices[0]) * vertices.size();
	}

	uint32_t Object::getIndexBufferSize()
	{
		return sizeof(indices[0]) * indices.size();
	}

}