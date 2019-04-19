#ifndef VERTEX2D_HPP
#define VERTEX2D_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <array>

namespace Obtain::Graphics::Vulkan {
	struct Vertex2D {
		glm::vec2 pos;
		glm::vec3 color;

		static vk::VertexInputBindingDescription getBindingDescription() {
			vk::VertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex2D);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;
			return bindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[1].offset = offsetof(Vertex2D, color);

			return attributeDescriptions;
		}

		bool operator==(const Vertex2D &other) const {
			return pos == other.pos && color == other.color;
		}
	};
}

#endif // VERTEX2D_HPP
