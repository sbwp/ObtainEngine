#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

#include "vertex.hpp"
#include "model.hpp"
#include "image.hpp"

namespace Obtain::Graphics::Vulkan {
	class Object {
	public:
		// Object();
		Object(Device *device, vk::UniqueCommandPool &commandPool, const std::string &modelFile,
		       const std::string &textureFile);

		static std::unique_ptr<Object> unique(Device *device, vk::UniqueCommandPool &commandPool,
		                                      const std::string &modelFile, const std::string &textureFile);

		std::vector<Vertex> &getVertices();

		std::vector<uint32_t> &getIndices();

		std::unique_ptr<Image> &getTextureImage();

		vk::DeviceSize getVertexBufferSize();

		vk::DeviceSize getIndexBufferSize();

		vk::DeviceSize getBufferSize();

		vk::DeviceSize getVertexBufferOffset();

		vk::DeviceSize getIndexBufferOffset();

		vk::DeviceSize getBufferOffset();

	private:
		Device *device;
		std::unique_ptr<Model> model;
		std::vector<Vertex> &vertices;
		std::vector<uint32_t> &indices;
		std::unique_ptr<Image> textureImage;
		vk::UniqueCommandPool &commandPool;
		std::unique_ptr<Buffer> buffer;

		std::unique_ptr<Buffer> createBuffer();
	};
}

#endif // OBJECT_HPP
