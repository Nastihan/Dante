#pragma once
#include "Pch.h"

namespace Dante::Core
{
	class Window
	{
	public:
		Window(UINT height, UINT width, std::string name);
		void Init();

	private:
		UINT height;
		UINT width;
		std::string name;
	};
}