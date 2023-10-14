#include "Pch.h"
#include "Core/Application.h"

int main(int agrc, char* argv[])
{
	try
	{
		auto app = std::make_unique<Dante::Core::Application>();
		app->Init();
		app->Run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}