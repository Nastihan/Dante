#include "Pch.h"
#include "Core//Window.h"
#include "Utils/NastihanUtil.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			::PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
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

		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (RegisterClass(&wc) < 0)
		{
			throw std::runtime_error(
				"failed to register wnd class"
			);
		}

		RECT r = { 0, 0, LONG(width), LONG(height) };
		AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
		int WIDTH = r.right - r.left;
		int HEIGHT = r.bottom - r.top;

		hWnd = ::CreateWindow(L"MainWnd", Utils::ToWide(name).c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, 0, 0, hInst, 0);

		if (hWnd == nullptr)
		{
			throw std::runtime_error(
				"failed to create the wnd"
			);
		}

		::ShowWindow(hWnd, SW_SHOW);
		::UpdateWindow(hWnd);

	}
	std::optional<int> Window::ProcessMessages() noexcept
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return (int)msg.wParam;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return {};
	}
}