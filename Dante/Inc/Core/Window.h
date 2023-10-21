#pragma once
#include "Utils/Singleton.h"
namespace Dante::Core
{
	class Window : public Utils::Singleton<Window>
	{
	public:
		Window(UINT width, UINT height, std::string name);
		void Init();
		static std::optional<int> ProcessMessages() noexcept;
		UINT GetWidth();
		UINT GetHeight();
		HWND GetHWnd();
	private:
		HWND hWnd;

		UINT width{};
		UINT height{};
		std::string name;
	};
}