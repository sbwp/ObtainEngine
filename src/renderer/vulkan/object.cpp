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
	{}

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