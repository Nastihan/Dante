#include "Pch.h"
#include "Core//Window.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			::PostQuitMessage(0);
		}break;

		default:
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}break;
	}
}

namespace Dante::Core
{
	Window::Window( UINT width, UINT height, std::string name)
		:width(width) , height(height), name(name)
	{
	}

	void Window::Init()
	{
		HINSTANCE hInst = GetModuleHandle(NULL);

		WNDCLASSEX wClass{};

		wClass.cbSize = sizeof(wClass);
		wClass.style = CS_HREDRAW | CS_VREDRAW;
		wClass.lpfnWndProc = WndProc;
		wClass.hInstance = hInst;
		wClass.cbClsExtra = 0;
		wClass.cbWndExtra = 0;
		wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		wClass.hCursor = LoadCursor(0, IDC_ARROW);
		wClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wClass.lpszMenuName = 0;
		wClass.lpszClassName = LPCWSTR(name.c_str());

		if (RegisterClassExW(&wClass) > 0)
		{
			throw std::runtime_error(
				"failed to register wnd class"
			);
		}

	}
}