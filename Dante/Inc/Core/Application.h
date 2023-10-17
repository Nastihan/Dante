#pragma once
#include "Utils/Singleton.h"
#include "Utils/Timer.h"
#include "Core/Window.h"

namespace Dante::Core
{
	class Application : public Utils::Singleton<Application>
	{
	public:
		Application() = default;
		~Application() = default;
		void Init();
		int Run();
	private:
		void Render();

	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<Utils::Timer> timer;
	};
}