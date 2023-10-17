#pragma once

namespace Dante::Core
{
	class Window
	{
	public:
		Window(UINT width, UINT height, std::string name);
		void Init();

	private:
		HWND hWnd;

		UINT width;
		UINT height;
		std::string name;
	};
}