#ifndef OBTAIN_GRAPHICS_VULKAN_UNIFORM_BUFFER_OBJECT_HPP
#define OBTAIN_GRAPHICS_VULKAN_UNIFORM_BUFFER_OBJECT_HPP

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Obtain::Graphics::Vulkan {
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
	};
}
#endif // OBTAIN_GRAPHICS_VULKAN_UNIFORM_BUFFER_OBJECT_HPP
