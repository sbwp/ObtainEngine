#include "device.hpp"

#include <vector>
#include <set>
#include <map>
#include <memory>

#include "swapchain-support-details.hpp"
#include "queue-family-indices.hpp"
#include "validation.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	vk::PhysicalDevice Device::selectPhysicalDevice(
			vk::UniqueInstance &instance,
			vk::UniqueSurfaceKHR &surface
	) {
		std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();
		if (devices.size() == 0) {
			throw std::runtime_error("no supported GPUs found");
		}

		std::multimap<uint32_t, vk::PhysicalDevice> deviceMap;

		for (const auto &device : devices) {
			deviceMap.insert(
					std::make_pair(
							ratePhysicalDeviceSuitability(
									device,
									*surface
							),
							device
					));
		}

		if (deviceMap.rbegin()
		             ->first > 0) {
			return deviceMap.rbegin()
			                ->second;
		} else {
			throw std::runtime_error("no suitable GPUs found");
		}
	}

	vk::UniqueDevice Device::createLogicalDevice(
			vk::UniqueInstance &instance,
			vk::PhysicalDevice physicalDevice,
			vk::UniqueSurfaceKHR &surface
	) {
		QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(
				physicalDevice,
				*surface
		);
		const float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
				indices.graphicsFamily
				       .value(),
				indices.presentFamily
				       .value()
		};

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			queueCreateInfos.push_back(
					vk::DeviceQueueCreateInfo(
							vk::DeviceQueueCreateFlags(),
							queueFamily,
							1U,
							&queuePriority
					)
			);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
		std::vector<const char *> validationLayers = Validation::getValidationLayers();

		return physicalDevice.createDeviceUnique(
			vk::DeviceCreateInfo(
				vk::DeviceCreateFlags(),
				static_cast<uint32_t>(queueCreateInfos.size()),
				queueCreateInfos.data(),
				static_cast<uint32_t>(validationLayers.size()),
				validationLayers.size() == 0 ? (const char *const *) nullptr : validationLayers.data(),
				static_cast<uint32_t>(deviceExtensions.size()),
				deviceExtensions.data(),
				&deviceFeatures
			)
		);
	}

	vk::UniqueSurfaceKHR Device::createSurface(const vk::Instance instance, GLFWwindow *window) {
		VkSurfaceKHR surface;

		if (glfwCreateWindowSurface(
				instance,
				window,
				nullptr,
				&surface
		) != VK_SUCCESS) {
			throw std::runtime_error("could not create surface");
		}
		return vk::UniqueSurfaceKHR(
				surface,
				instance
		);
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/

	const std::vector<const char *> Device::deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	uint32_t Device::ratePhysicalDeviceSuitability(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		// Get device properties
		auto deviceProperties = device.getProperties();

		// Get device features
		auto deviceFeatures = device.getFeatures();

		uint32_t score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits
		                         .maxImageDimension2D;

		// Check if required Extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		// Check for sufficient swap chains
		bool swapchainAdequate = false;
		if (extensionsSupported) {
			SwapchainSupportDetails swapchainSupport = SwapchainSupportDetails::querySwapchainSupport(
					device,
					surface
			);
			swapchainAdequate = !swapchainSupport.formats
			                                     .empty() && !swapchainSupport.presentModes
			                                                                  .empty();
		}

		// Check for missing features that are complete dealbreakers
		if (!(deviceFeatures.geometryShader && QueueFamilyIndices::findQueueFamilies(
				device,
				surface
		).isComplete() &&
		      extensionsSupported && swapchainAdequate && deviceFeatures.samplerAnisotropy)) {
			// TODO: instead of disqualifying for missing Anistropy here, conditionally use it
			return 0;
		}

		return score;
	}

	bool Device::checkDeviceExtensionSupport(vk::PhysicalDevice device) {

		std::vector<vk::ExtensionProperties> availableExtensions;
		availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(
				deviceExtensions.begin(),
				deviceExtensions.end());

		for (const auto &extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}
