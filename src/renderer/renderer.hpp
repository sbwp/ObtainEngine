#ifndef RENDERER_HPP
#define RENDERER_HPP

namespace Obtain::Graphics {
	class Renderer {
		public:
		void run();
		
		protected:
		virtual void init();
		virtual void mainLoop();
		virtual void cleanup();
		
	};
}

#endif // RENDERER_HPP