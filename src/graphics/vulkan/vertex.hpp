#ifndef OBTAIN_GRAPHICS_VULKAN_VERTEX_HPP
#define OBTAIN_GRAPHICS_VULKAN_VERTEX_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <array>

namespace Obtain::Graphics::Vulkan {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			vk::VertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;
			return bindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {
				vk::VertexInputAttributeDescription(0, 0,
				                                    vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
				vk::VertexInputAttributeDescription(1, 0,
				                                    vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
				vk::VertexInputAttributeDescription(2, 0,
				                                    vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
			};

			return attributeDescriptions;
		}

		float distance(Vertex v)
		{
			float dx = pos.x - v.pos
				.x;
			float dy = pos.y - v.pos
				.y;
			float dz = pos.z - v.pos
				.z;

			return (float) sqrt(dx * dx + dy * dy + dz * dz);
		}

		bool operator==(const Vertex &other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};
}


namespace std {
	template<>
	struct hash<Obtain::Graphics::Vulkan::Vertex> {
		size_t operator()(Obtain::Graphics::Vulkan::Vertex const &vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
			         (hash<glm::vec3>()(vertex.color) << 1U)) >> 1U) ^
			       (hash<glm::vec2>()(vertex.texCoord) << 1U);
		}
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_VERTEX_HPP
