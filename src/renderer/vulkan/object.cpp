//
// Created by austin on 4/19/19.
//

#include "object.hpp"

#include "device.hpp"

#define TEXTURE_LOCATION "assets/textures/"

namespace Obtain::Graphics::Vulkan {

	Object::Object(Device *device, vk::UniqueCommandPool &commandPool, const std::string &modelFile,
	               const std::string &textureFile)
		: model(Model::unique(modelFile)), vertices(model->getVertices()), indices(model->getIndices()),
		  textureImage(Image::createTextureImage(device, commandPool, textureFile))
	{

		// std::vector<Vertex> vertexes = {
		// 	(Vertex){{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		// 	(Vertex){{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		// 	(Vertex){{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		// 	(Vertex){{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		//
		// 	(Vertex){{-0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		// 	(Vertex){{ 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		// 	(Vertex){{ 0.5f,  0.5f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		// 	(Vertex){{-0.5f,  0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		//
		// 	(Vertex){{-0.5f, -0.5f, -1.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		// 	(Vertex){{ 0.5f, -0.5f, -1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		// 	(Vertex){{ 0.5f,  0.5f, -1.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		// 	(Vertex){{-0.5f,  0.5f, -1.5f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		//
		// 	(Vertex){{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		// 	(Vertex){{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		// 	(Vertex){{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
		// 	(Vertex){{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
		// };
		//
		// std::vector<uint32_t> indexes = {
		// 	4, 5, 6,
		// 	6, 7, 4,
		// 	1, 0, 3,
		// 	3, 2, 1,
		// 	1, 5, 4,
		// 	4, 0, 1,
		// 	0, 4, 7,
		// 	7, 3, 0,
		// 	2, 6, 5,
		// 	5, 1, 2,
		// 	3, 7, 6,
		// 	6, 2, 3,
		//
		// 	12, 13, 14,
		// 	14, 15, 12,
		// 	9, 8, 11,
		// 	11, 10, 9,
		// 	9, 13, 12,
		// 	12, 8, 9,
		// 	8, 12, 15,
		// 	15, 11, 8,
		// 	10, 14, 13,
		// 	13, 9, 10,
		// 	11, 15, 14,
		// 	14, 10, 11,
		// };
		//
		// vertices = vertexes;
		// indices = indexes;
	}

	std::unique_ptr<Object> Object::unique(Device *device, vk::UniqueCommandPool &commandPool,
	                                       const std::string &modelFile, const std::string &textureFile)
	{
		return std::make_unique<Object>(Object(device, commandPool,
		                                       modelFile, textureFile));
	}

	std::vector<Vertex> &Object::getVertices()
	{
		return vertices;
	}

	std::vector<uint32_t> &Object::getIndices()
	{
		return indices;
	}
	std::unique_ptr<Image> &Object::getTextureImage()
	{
		return textureImage;
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