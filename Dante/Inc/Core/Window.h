#pragma once
#include "Pch.h"

namespace Dante::Core
{
	class Window
	{
	public:
		Window(UINT width, UINT height, std::string name);
		void Init();

	private:
		UINT width;
		UINT height;
		std::string name;
	};
}