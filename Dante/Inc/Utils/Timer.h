#pragma once
#include <chrono>

namespace Dante::Utils
{
	class Timer
	{
	public:
		Timer();
		float Peek();
		float Mark();

	private:
		std::chrono::steady_clock::time_point last;
	};
}