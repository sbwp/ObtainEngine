#ifndef DEVICE_HPP
#define DEVICE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Device {
	public:
		static vk::PhysicalDevice selectPhysicalDevice(
			vk::UniqueInstance &instance,
			vk::UniqueSurfaceKHR &surface
		);
		static vk::UniqueDevice createLogicalDevice(
			vk::UniqueInstance &instance,
			vk::PhysicalDevice physicalDevice,
			vk::UniqueSurfaceKHR &surface
		);
		static vk::UniqueSurfaceKHR createSurface(const vk::Instance instance, GLFWwindow *window);
	private:
		static uint32_t ratePhysicalDeviceSuitability(vk::PhysicalDevice device, vk::SurfaceKHR surface);
		static bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
		
	};
}

#endif // DEVICE_HPP