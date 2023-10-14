#pragma once
#include <chrono>
#include "Utils/Singleton.h"
namespace Dante::Utils
{
	class Timer : public Singleton<Timer>
	{
	public:
		Timer();
		float Peek();
		float Mark();

	private:
		std::chrono::steady_clock::time_point last;
	};
}