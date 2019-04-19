#ifndef DEVICE_HPP
#define DEVICE_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace Obtain::Graphics::Vulkan {
	class Device {
	public:
		static std::unique_ptr<vk::PhysicalDevice> selectPhysicalDevice(
				vk::UniqueInstance &instance,
				vk::UniqueSurfaceKHR &surface
		);

		static vk::UniqueDevice createLogicalDevice(
				vk::UniqueInstance &instance,
				std::unique_ptr<vk::PhysicalDevice> &physicalDevice,
				vk::UniqueSurfaceKHR &surface
		);

		static vk::UniqueSurfaceKHR createSurface(const vk::Instance instance, GLFWwindow *window);

		static uint32_t findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	private:
		static const std::vector<const char *> deviceExtensions;

		static uint32_t ratePhysicalDeviceSuitability(vk::PhysicalDevice device, vk::UniqueSurfaceKHR &surface);

		static bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

	};
}

#endif // DEVICE_HPP
