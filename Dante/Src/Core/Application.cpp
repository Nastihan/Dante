#include "Pch.h"
#include "Core/Application.h"

namespace Dante::Core
{
	void Application::Init()
	{
		timer = std::make_unique<Utils::Timer>();
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