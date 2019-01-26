#ifndef GRAPHICSPIPELINE_HPP
#define GRAPHICSPIPELINE_HPP

#include <vulkan/vulkan.hpp>
//#include <vector>

namespace Obtain::Graphics::Vulkan {
	class GraphicsPipeline {
		public:
			GraphicsPipeline();
			~GraphicsPipeline();
		private:
			vk::Device device;
			vk::Extent2D swapchainExtent;
			vk::SampleCountFlagBits msaaSamples;
			vk::DescriptorSetLayout descriptorSetLayout;
			vk::PipelineLayout pipelineLayout;
			vk::Pipeline graphicsPipeline;
			vk::RenderPass renderPass;
			
			void createImageViews();
			void createRenderPass();
	};
	
}
#endif //GRAPHICSPIPELINE_HPP
