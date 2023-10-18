#pragma once
#include "Pch.h"

namespace Dante::Rendering
{
	class Renderer
	{
	public:
		Renderer() = default;
		DISABLE_COPY_AND_MOVE(Renderer)
		void Init();
		void Render();
	private:

	};
}