#include "vulkanrenderer.hpp"

namespace Obtain::Graphics::Vulkan {
	/******************************************
	 ***************** public *****************
	 ******************************************/
	VulkanRenderer::VulkanRenderer(std::string title,std::array<uint32_t, 3> version)
		: title(title), version(version)
	{
		windowSize = {1600, 900};
		initWindow();
	}
	
	VulkanRenderer::~VulkanRenderer()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VulkanRenderer::run()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	/******************************************
	 ***************** private *****************
	 ******************************************/
	
	void VulkanRenderer::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't create OpenGL context
		glfwWindowHint(GLFW_RESIZABLE, false); // Temp: make window not resizable
		window = glfwCreateWindow(windowSize[0], windowSize[1], title.c_str(), nullptr, nullptr);
	}
}
