#include "instance.hpp"

#include <iostream>

#include "validation.hpp"

namespace Obtain::Graphics::Vulkan {

	/******************************************
	 ***************** public *****************
	 ******************************************/
	vk::UniqueInstance Instance::createVkInstance(
			std::string gameTitle,
			std::array<uint32_t, 3> gameVersion,
			std::array<uint32_t, 3> engineVersion
	) {
		vk::ApplicationInfo appInfo(
				gameTitle.c_str(),
				VK_MAKE_VERSION(gameVersion[0],
				                gameVersion[1],
				                gameVersion[2]),
				"Obtain Engine",
				VK_MAKE_VERSION(engineVersion[0],
				                engineVersion[1],
				                engineVersion[2]), // engine gameVersion (replace with not constant)
				VK_API_VERSION_1_1
		);

		bool useValidationLayers = Validation::useValidation();
		std::vector<const char *> requiredExtensions = getRequiredExtensions(useValidationLayers);
		if (!checkForSupportedExtensions(requiredExtensions)) {
			throw std::runtime_error("Required Extensions not supported");
		}

		vk::InstanceCreateInfo createInfo(
				vk::InstanceCreateFlags(),
				&appInfo,
				useValidationLayers ? static_cast<uint32_t>(Validation::validationLayers.size()) : 0U,
				useValidationLayers ? Validation::validationLayers.data() : nullptr,
				static_cast<uint32_t>(requiredExtensions.size()),
				requiredExtensions.data()
		);

		return vk::createInstanceUnique(createInfo);
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	bool Instance::checkForSupportedExtensions(std::vector<const char *> requiredExtensions) {
		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

		// Print all available extensions
		std::cout << "available extensions:" << std::endl;
		for (const auto &extension : extensions) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		// Check each required extension for availability
		for (const auto &requiredExtension : requiredExtensions) {
			bool found = false;
			for (const auto &extension : extensions) {
				if (strcmp(
						extension.extensionName,
						requiredExtension
				) == 0) {
					found = true;
					break;
				}
			}
			if (!found) {
				std::cerr << "Required Extension " << requiredExtension << " not found." << std::endl;
				return false;
			}
		}
		return true;
	}

	std::vector<const char *> Instance::getRequiredExtensions(bool useValidationLayers) {
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(
				glfwExtensions,
				glfwExtensions + glfwExtensionCount
		);

		if (useValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
}
