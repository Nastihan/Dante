#pragma once
//#include "Pch.h"

namespace Dante::Core
{
	class Window
	{
	public:
		Window(UINT width, UINT height, std::string name);
		void Init();
		static std::optional<int> ProcessMessages() noexcept;

	private:
		HWND hWnd;

		UINT width;
		UINT height;
		std::string name;
	};
}