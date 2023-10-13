#include "Pch.h"
#include "Core/Application.h"

int main(int agrc, char* argv[])
{
	auto app = std::make_unique<Dante::Core::Application>();
	app->Init();
	app->Run();

	return 0;
}