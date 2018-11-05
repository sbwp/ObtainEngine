#include <vulkan/vulkan.hpp>
//#include <vector>

namespace Obtain::Graphics::Vulkan {
	class GraphicsPipeline {
		public:
			GraphicsPipeline();
			~GraphicsPipeline();
		private:
			vk::Device device;
			
	};
	
}