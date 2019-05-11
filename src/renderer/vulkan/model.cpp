//
// Created by austin on 5/4/19.
//

#include <memory>
#include "model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define MODEL_LOCATION "assets/models/"

namespace Obtain::Graphics::Vulkan {
	Model::Model(const std::string &source)
	{
		auto filename = MODEL_LOCATION + source;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err,
		                      filename.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices;

		for (const auto &shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex = {};
				vertex.pos = {
					attributes.vertices[3 * index.vertex_index],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2]
				};
				vertex.texCoord = {
					attributes.texcoords[2 * index.texcoord_index],
					1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
				};
				vertex.color = {1.0f, 1.0f, 1.0f};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.emplace_back(vertex);
				}

				indices.emplace_back(uniqueVertices[vertex]);
			}
		}
	}

	std::unique_ptr<Model> Model::unique(const std::string &source)
	{
		return std::make_unique<Model>(Model(source));
	}

	std::vector<Vertex> &Model::getVertices()
	{
		return vertices;
	}
	std::vector<uint32_t> &Model::getIndices()
	{
		return indices;
	}
}