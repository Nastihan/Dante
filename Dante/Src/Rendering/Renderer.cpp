#include "Pch.h"
#include "Rendering/Renderer.h"

namespace Dante::Rendering
{

	void Renderer::Init()
	{
		gfx = std::make_unique<Graphics>();
		gfx->Init();
	}
	void Renderer::Render()
	{
	}
}