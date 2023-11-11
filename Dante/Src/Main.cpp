#include "Pch.h"
#include "Core/Application.h"

int main(int agrc, char* argv[])
{
	try
	{
		auto app = std::make_unique<Dante::Core::Application>();
		Dante::Core::Application::Instance().Init();
		return Dante::Core::Application::Instance().Run();
	}

	catch (const Dante::Utils::NastihanException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"NastihanException : HR Failed", MB_OK);
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "StandardException", MB_ICONERROR | MB_SETFOREGROUND);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);

	}
	return -1;
}

