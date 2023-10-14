#pragma once
#include "Utils/Timer.h"

namespace Dante::Core
{
	class Application
	{
	public:
		Application() = default;
		~Application() = default;
		void Init();
		void Run();
	private:
		void Render();

		std::unique_ptr<Utils::Timer> timer;
	};
}