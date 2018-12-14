#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <array>

#include "graphicspipeline.hpp"

namespace Obtain::Graphics::Vulkan {
	class Instance {
		public:
			Instance(std::string gameTitle, std::array<uint32_t, 3> gameVersion);
			~Instance();
		private:
			std::string title;
			uint32_t version;
			GLFWwindow* window;
			std::array<int, 2> screenDimensions = {800, 600};
			bool framebufferResized = false;
			
			vk::UniqueInstance instance;
			vk::Optional<const vk::AllocationCallbacks> allocator = nullptr;
			
			vk::SurfaceKHR surface;
			vk::PhysicalDevice physicalDevice;
			vk::UniqueDevice device;
			GraphicsPipeline graphicsPipeline;

			const std::vector<const char*> validationLayers = {
				"VK_LAYER_LUNARG_standard_validation"
			};
			
			#ifdef NDEBUG
				const bool enableValidationLayers = false;
			#else
				const bool enableValidationLayers = true;
			#endif
			
			void initWindow();
			
			std::vector<const char*> getRequiredExtensions();
			
			bool checkValidationLayerSupport();
			
			void setupDebugCallback();
			
			// static VKAPI_ATTR bool VKAPI_CALL debugCallback(
			// 	VkDebugReportFlagsEXT flags,
			// 	VkDebugReportObjectTypeEXT objType,
			// 	uint64_t obj,
			// 	size_t location,
			// 	int32_t code,
			// 	const char* layerPrefix,
			// 	const char* msg,
			// 	void* userData
			// );
			
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}

#endif // INSTANCE_HPP