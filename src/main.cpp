#include <iostream>
//#include "renderer/vulkan/vulkanrenderer.hpp"
#include "renderer/vulkan/instance.hpp"

int main() {
	auto instance = new Obtain::Graphics::Vulkan::Instance("Hi", {1, 0, 0});
	// Obtain::Graphics::Vulkan::VulkanRenderer renderer;

	// try {
	// 	renderer.run();
	// }
	// catch (const std::runtime_error& e) {
	// 	std::cerr << e.what() << std::endl;
	// 	return EXIT_FAILURE;
	// }

	// return EXIT_SUCCESS;
}
