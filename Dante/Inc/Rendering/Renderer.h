#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
#include "Utils/DXUtil.h"
#include "Utils/NastihanMath.h"
#include "Rendering/RHI/UploadBuffer.h"
#include <Rendering/RHI/VertexBuffer.h>
#include <Rendering/RHI/IndexBuffer.h>
#include "Scene/Camera.h"
#include "Scene/Model.h"
#include "Scene/SkySphere.h"

namespace Dante::Rendering
{

	
	
	class Renderer
	{
	public:
		Renderer() = default;
		DISABLE_COPY_AND_MOVE(Renderer)
		void Init();
		void OnResize();
		void Update(float dt);
		void Render();
		void ShowFpsWindow();
		Graphics& Gfx()
		{
			if (!gfx)
				throw std::runtime_error("No Gfx!");
			return *gfx;
		}
	private:
		void BeginFrame();
		void EndFrame();
	private:

	private:
		std::unique_ptr<Graphics> gfx{};
		std::unique_ptr<Scene::Camera> camera;
		Dante::Utils::PassConstants passConstants;
		std::unique_ptr<RHI::UploadBuffer<Dante::Utils::PassConstants>> passCB = nullptr;

		std::unique_ptr<Scene::Model> sponza;
		std::unique_ptr<Scene::Model> helmet;
		std::unique_ptr<Scene::Model> aBeautifulGame;
		std::unique_ptr<Scene::SkySphere> skySphere;
		




	};
}