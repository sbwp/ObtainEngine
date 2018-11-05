#include "renderer.hpp"

namespace Obtain::Graphics {
	void Renderer::run() {
		init();
		mainLoop();
		cleanup();
	}
}