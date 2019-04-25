//
// Created by austin on 4/19/19.
//

#include "object.hpp"
#include "device.hpp"

namespace Obtain::Graphics::Vulkan {

	Object::Object()
	{
		vertices = {
			{{0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
		};
	}

	std::vector<Vertex2D> Object::getVertices()
	{
		return vertices;
	}

	uint32_t Object::getBufferSize()
	{
		return sizeof(vertices[0]) * vertices.size();
	}

}