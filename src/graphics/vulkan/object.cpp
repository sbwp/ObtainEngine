//
// Created by sabrina on 4/19/19.
//

#include "object.hpp"

#include "device.hpp"
#include "buffer.hpp"

#define TEXTURE_LOCATION "assets/textures/"

namespace Obtain::Graphics::Vulkan {

	/******************************************************
	 ******************* public **************************
	 *****************************************************/

	Object::Object(Device *device, vk::UniqueCommandPool &commandPool, const std::string &modelFile,
	               const std::string &textureFile)
		: model(Model::unique(modelFile)), vertices(model->getVertices()), indices(model->getIndices()),
		  textureImage(Image::createTextureImage(device, commandPool, textureFile)),
		  device(device), commandPool(commandPool), buffer(createBuffer())
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

	vk::DeviceSize Object::getVertexBufferSize()
	{
		return static_cast<vk::DeviceSize>(sizeof(vertices[0]) * vertices.size());
	}

	vk::DeviceSize Object::getIndexBufferSize()
	{
		return static_cast<vk::DeviceSize>(sizeof(indices[0]) * indices.size());
	}

	vk::DeviceSize Object::getBufferSize()
	{
		return getVertexBufferSize() + getIndexBufferSize();
	}

	vk::DeviceSize Object::getVertexBufferOffset()
	{
		return buffer->getOffset() + getIndexBufferSize();
	}

	vk::DeviceSize Object::getIndexBufferOffset()
	{
		return getBufferOffset();
	}

	vk::DeviceSize Object::getBufferOffset()
	{
		return buffer->getOffset();
	}

	/******************************************************
	 ******************* private **************************
	 *****************************************************/

	std::unique_ptr<Buffer> Object::createBuffer()
	{
		vk::DeviceSize indicesSize = indices.size() * sizeof(uint32_t);
		vk::DeviceSize verticesSize = vertices.size() * sizeof(Vertex);
		vk::DeviceSize size = indicesSize + verticesSize;

		Buffer stagingBuffer = Buffer(
			device,
			size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		stagingBuffer.load(0u, indices.data(), static_cast<size_t>(indicesSize));
		stagingBuffer.load(indicesSize, vertices.data(), static_cast<size_t>(verticesSize));

		auto newBuffer = Buffer::unique(
			device,
			size,
			vk::BufferUsageFlagBits::eTransferDst |
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		stagingBuffer.copyToBuffer(commandPool, device->getGraphicsQueue(), newBuffer);
		return newBuffer;
	}
}