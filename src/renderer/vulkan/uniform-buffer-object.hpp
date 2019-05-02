#ifndef UNIFORM_BUFFER_OBJECT_HPP
#define UNIFORM_BUFFER_OBJECT_HPP

#include <glm/glm.hpp>

namespace Obtain::Graphics::Vulkan {
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
	};
}
#endif // UNIFORM_BUFFER_OBJECT_HPP
