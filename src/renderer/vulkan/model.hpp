//
// Created by austin on 5/4/19.
//

#ifndef OBTAIN_MODEL_HPP
#define OBTAIN_MODEL_HPP

#include "vertex.hpp"

#include <tiny_obj_loader.h>

namespace Obtain::Graphics::Vulkan {
	class Model {
	public:
		explicit Model(const std::string &source);
		static std::unique_ptr<Model> unique(const std::string &source);

		std::vector<Vertex> &getVertices();
		std::vector<uint32_t> &getIndices();

	private:
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	};
}

#endif // OBTAIN_MODEL_HPP
