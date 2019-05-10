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

		uint32_t getBufferSize();

		uint32_t getIndexBufferSize();

	private:
		std::unique_ptr<Model> model;
		std::vector<Vertex> &vertices;
		std::vector<uint32_t> &indices;
		std::unique_ptr<Image> textureImage;
	};
}

#endif // OBJECT_HPP
