#pragma once
#include "Utils/Singleton.h"
#include "Utils/Timer.h"
#include "Core/Window.h"
#include "Rendering/Renderer.h"

namespace Dante::Core
{
	class Application : public Utils::Singleton<Application>
	{
	public:
		Application() = default;
		DISABLE_COPY_AND_MOVE(Application)
		~Application() = default;
		void Init();
		int Run();
		void OnResize();

	private:
		void Render();
		void Update();
	public:
		bool RendererInitialized();

	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<Rendering::Renderer> renderer;
		std::unique_ptr<Utils::Timer> timer;
	};
}