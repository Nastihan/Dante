#pragma once
#include "Utils/Singleton.h"
namespace Dante::Core
{
	class Window : public Utils::Singleton<Window>
	{
	public:
		Window(UINT width, UINT height, std::string name);
		void Init();
		static LRESULT MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static std::optional<int> ProcessMessages() noexcept;
		UINT GetWidth();
		UINT GetHeight();
		float GetAR();
		HWND GetHWnd();
	private:
		HWND hWnd;

		UINT width{};
		UINT height{};
		std::string name;
	};
}