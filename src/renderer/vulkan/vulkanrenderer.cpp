#include "vulkanrenderer.hpp"

namespace Obtain::Graphics::Vulkan {
	/****** public ******/
	
	VulkanRenderer::VulkanRenderer(std::string title,std::array<uint32_t, 3> version)
		: title(title), version(version)
	{
		instance = new Instance(title, version);
	}
	
	VulkanRenderer::~VulkanRenderer()
	{
		delete(instance);
	}

	void VulkanRenderer::run()
	{
		instance->run();
	}

	/****** private ******/
	
}
