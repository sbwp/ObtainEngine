#ifndef OBTAIN_GRAPHICS_VULKAN_VALIDATION_HPP
#define OBTAIN_GRAPHICS_VULKAN_VALIDATION_HPP

#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Validation {
	public:
		static const std::vector<const char *> validationLayers;

		static bool useValidation();

		static vk::DebugUtilsMessengerEXT createDebugMessenger(
			vk::UniqueInstance &instance,
			vk::DispatchLoaderDynamic loader
		);

		static std::vector<const char *> getValidationLayers();

	private:
		static bool areValidationLayersSupported();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
			void *pUserData
		);
	};
}

#endif // OBTAIN_GRAPHICS_VULKAN_VALIDATION_HPP
