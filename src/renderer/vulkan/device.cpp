#include <map>
#include <set>
#include <vector>

#include "device.hpp"
#include "queuefamilyindices.hpp"
#include "swapchainsupportdetails.hpp"

namespace Obtain::Graphics::Vulkan {
	Device::Device(vk::Instance instance, std::vector<char*> deviceExtensions,
		bool enableValidationLayers, std::vector<char*> validationLayers,
		vk::SurfaceKHR surface
	) {
		physicalDevice = choosePhysicalDevice(instance, surface);
		
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = 
			{ indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			auto queueCreateInfo = vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlagBits(),
				queueFamily, 1, &queuePriority);
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures *deviceFeatures = new vk::PhysicalDeviceFeatures();
		deviceFeatures->samplerAnisotropy = true;
		deviceFeatures->sampleRateShading = true;

		auto createInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlagBits(),
			static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
			enableValidationLayers? static_cast<uint32_t>(validationLayers.size()) :  0,
			enableValidationLayers? validationLayers.data() : nullptr,
			static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), deviceFeatures
		);

		auto graphicsQueue = device->getQueue(indices.graphicsFamily.value(), 0);
		auto presentQueue = device->getQueue(indices.presentFamily.value(), 0);
		
		device = physicalDevice.createDeviceUnique(createInfo);
	}
	
	Device::~Device() {
		
	}
	
	vk::PhysicalDevice Device::choosePhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface) {
		uint32_t deviceCount = 0;
		instance.enumeratePhysicalDevices(&deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support");
		}

		std::vector<vk::PhysicalDevice> devices(deviceCount);
		instance.enumeratePhysicalDevices(&deviceCount, devices.data());

		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, vk::PhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = rateDeviceSuitability(device, surface);
			candidates.insert(std::make_pair(score, device));
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0) {
			physicalDevice = candidates.rbegin()->second;
		}
		else {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}
	
	// Check maximum MSAA samples
	vk::SampleCountFlagBits Device::getMaxUsableSampleCount() {
		vk::PhysicalDeviceProperties physicalDeviceProperties = physicalDevice.getProperties();
		vk::SampleCountFlags counts =
			static_cast<uint32_t>(physicalDeviceProperties.limits.framebufferColorSampleCounts)
			< static_cast<uint32_t>(physicalDeviceProperties.limits.framebufferDepthSampleCounts) 
			? physicalDeviceProperties.limits.framebufferColorSampleCounts
			: physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
		if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
		if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
		if (counts & vk::SampleCountFlagBits::e8)  { return vk::SampleCountFlagBits::e8; }
		if (counts & vk::SampleCountFlagBits::e4)  { return vk::SampleCountFlagBits::e4; }
		if (counts & vk::SampleCountFlagBits::e2)  { return vk::SampleCountFlagBits::e2; }

		return vk::SampleCountFlagBits::e1;
	}
	
	

	// Check if a device has the properties and features needed to run this application
	int Device::rateDeviceSuitability(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		// Get device properties
		auto deviceProperties = physicalDevice.getProperties();

		// Get device features
		auto deviceFeatures = physicalDevice.getFeatures();
		
		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Check if required Extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		// Check for sufficient swap chains
		bool swapchainAdequate = false;
		if (extensionsSupported) {
			SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
			swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
		}

		// Check for missing features that are complete dealbreakers
		if (!(deviceFeatures.geometryShader && findQueueFamilies(device, surface).isComplete() &&
			extensionsSupported && swapchainAdequate && deviceFeatures.samplerAnisotropy))
		{
			// TODO: instead of disqualifying for missing Anistropy here, conditionally use it
			return 0;
		}

		return score;
	}
	
	SwapchainSupportDetails Device::querySwapchainSupport(vk::PhysicalDevice device,
		vk::SurfaceKHR surface
	) {
		SwapchainSupportDetails details;

		details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		// Check size of formats
		details.formats = physicalDevice.getSurfaceFormatsKHR(surface);

		details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		return details;
	}

	// Find queue families
	QueueFamilyIndices Device::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices;
		
		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {

			vk::Bool32 presentSupport = false;
			device.getSurfaceSupportKHR(i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			
			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	// Check device extension support
	bool Device::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
		
		std::vector<vk::ExtensionProperties> availableExtensions;
		availableExtensions = device.enumerateDeviceExtensionProperties();
		
		const std::vector <const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}