#include "Pch.h"
#include "Core/Application.h"

namespace Dante::Core
{
	void Application::Init()
	{
		timer = std::make_unique<Utils::Timer>();

		window = std::make_unique<Core::Window>(1600, 900, "Window");
		window->Init();
	}

	void Application::Run()
	{
		while (true)
		{
			auto dt = Utils::Timer::Instance().Mark();
			std::cout << dt << std::endl;
			Render();
		}
	}
	void Application::Render()
	{
	}
}