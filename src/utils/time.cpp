//
// Created by austin on 5/2/19.
//

#include "time.hpp"

namespace Obtain {
	float Time::elapsedTime()
	{
        static auto start = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float, std::chrono::seconds::period>(now - start).count();
	}
}
