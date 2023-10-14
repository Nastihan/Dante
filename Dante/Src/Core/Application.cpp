#include "Pch.h"
#include "Core/Application.h"

namespace Dante::Core
{
	void Application::Init()
	{
	}

	void Application::Run()
	{
		timer = std::make_unique<Utils::Timer>();

		while (true)
		{
			auto dt = timer->Mark();

			Render();
			std::cout << dt << std::endl;
		}
	}
	void Application::Render()
	{
	}
}