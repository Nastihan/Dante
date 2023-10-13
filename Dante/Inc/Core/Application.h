#pragma once

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
	};
}