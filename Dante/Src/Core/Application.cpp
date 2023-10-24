#include "Pch.h"
#include "Core/Application.h"

namespace Dante::Core
{
	void Application::Init()
	{
		timer = std::make_unique<Utils::Timer>();

		window = std::make_unique<Core::Window>(1600, 900, "My Window");
		window->Init();

		renderer = std::make_unique<Rendering::Renderer>();
		renderer->Init();
	}

	int Application::Run()
	{
		while (true)
		{
			if (const auto ecode = Window::ProcessMessages())
			{
				return *ecode;
			}

			auto dt = Utils::Timer::Instance().Mark();
			//std::cout << dt << std::endl;
			Update();
			Render();
		}
	}
	void Application::Render()
	{
		renderer->Render();
	}

	void Application::Update()
	{
		renderer->Update();
	}
}