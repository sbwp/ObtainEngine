#ifndef UNIFORM_BUFFER_OBJECT_HPP
#define UNIFORM_BUFFER_OBJECT_HPP

#include <glm/glm.hpp>
namespace Obtain::Graphics::Vulkan {
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}
#endif // UNIFORM_BUFFER_OBJECT_HPP
