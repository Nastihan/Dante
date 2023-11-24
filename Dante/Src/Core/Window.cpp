#include "Pch.h"
#include "Core//Window.h"
#include "Core/Application.h"
#include "../ThirdParty/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Dante::Core
{
	Window::Window( UINT width, UINT height, std::string name)
		:width(width) , height(height), name(name)
	{
	}

	Window::~Window()
	{
		ImGui_ImplWin32_Shutdown();
	}

	void Window::Init()
	{
		HINSTANCE hInst = GetModuleHandle(NULL);

		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
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

		InitImgui();
	}

	LRESULT Window::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
			return pWnd->MsgProc(hwnd, msg, wParam, lParam);
		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;
		//const auto& imio = ImGui::GetIO();
		


		switch (msg)
		{

			case WM_SIZE:
			{
				if (Core::Application::Instance().RendererInitialized())
				{
					width = LOWORD(lParam);
					height = HIWORD(lParam);
					Dante::Core::Application::Instance().OnResize();
				}
				else
				{
					return ::DefWindowProc(hwnd, msg, wParam, lParam);
				}
				return 0;

			}

			case WM_DESTROY:
			{
				::PostQuitMessage(0);
				return 0;
			}

			default:
			{
				return ::DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
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

	////////////////////////////////// Getters
	UINT Window::GetWidth()
	{
		return width;
	}

	UINT Window::GetHeight()
	{
		return height;
	}

	float Window::GetAR()
	{
		return static_cast<float>(width) / (height);
	}

	HWND Window::GetHWnd()
	{
		return hWnd;
	}

	void Window::InitImgui()
	{
		ImGui_ImplWin32_Init(hWnd);
	}
}

