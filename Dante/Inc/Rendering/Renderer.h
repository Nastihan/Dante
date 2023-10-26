#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
#include "Utils/DXUtil.h"


namespace Dante::Rendering
{
	
	class Renderer
	{
	public:
		Renderer() = default;
		DISABLE_COPY_AND_MOVE(Renderer)
		void Init();
		void OnResize();
		void Update();
		void Render();
	private:
		void BeginFrame();
		void EndFrame();
	private:
		// TEMP
		void LoadCube();
	private:
		std::unique_ptr<Graphics> gfx{};
		std::unique_ptr<Utils::MeshGeometry> cube;
		DirectX::XMMATRIX viewProj;
	};
}