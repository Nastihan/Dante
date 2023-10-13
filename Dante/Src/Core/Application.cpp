#include "Pch.h"
#include "Core/Application.h"

namespace Dante::Core
{
	void Application::Init()
	{
	}

	void Application::Run()
	{
		while (true)
		{
			std::cout << "hello" << std::endl;

			Render();
		}
	}
	void Application::Render()
	{
	}
}