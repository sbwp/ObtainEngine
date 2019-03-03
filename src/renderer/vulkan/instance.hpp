#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Instance {
	public:
		static vk::UniqueInstance createVkInstance(
			std::string gameTitle,
			std::array<uint32_t, 3> gameVersion,
			std::array<uint32_t, 3> engineVersion
		);
	
	private:
		static bool checkForSupportedExtensions(std::vector<const char*> requiredExtensions);
		static std::vector<const char*> getRequiredExtensions(bool useValidationLayers);
			
	};
}

#endif // INSTANCE_HPP