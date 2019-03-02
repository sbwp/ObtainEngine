#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../renderer.hpp"

namespace Obtain::Graphics::Vulkan {
	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer(std::string title, std::array<uint32_t, 3> version);
		~VulkanRenderer();
		void run();

	private:
		std::array<u_int32_t, 2> windowSize;
		GLFWwindow* window;
		vk::UniqueInstance instance;
		std::string title;
		std::array<uint32_t, 3> version;
		
		void initWindow();
	};
}

#endif // VULKAN_RENDERER_HPP