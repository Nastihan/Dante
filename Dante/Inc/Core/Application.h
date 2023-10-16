#pragma once
#include "Utils/Singleton.h"
#include "Utils/Timer.h"

namespace Dante::Core
{
	class Application : public Utils::Singleton<Application>
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