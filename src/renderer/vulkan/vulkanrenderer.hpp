#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "../renderer.hpp"
#include "instance.hpp"

namespace Obtain::Graphics::Vulkan {
	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer(std::string title, std::array<uint32_t, 3> version);
		~VulkanRenderer();
		void run();

	private:
		Instance *instance;
		std::string title;
		std::array<uint32_t, 3> version;

	};
}

#endif // VULKAN_RENDERER_HPP