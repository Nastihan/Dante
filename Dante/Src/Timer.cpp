#include "Pch.h"
#include "Utils/Timer.h"

namespace Dante::Utils
{
	Timer::Timer()
	{
	}

	float Timer::Peek()
	{
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
	}

	float Timer::Mark()
	{
		const auto old = last;
		last = std::chrono::steady_clock::now();
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - old).count();
	}


}