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
#include "Rendering/RHI/ShadowMap.h"

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
		Dante::Utils::DefaultPassConstants passConstants;
		std::unique_ptr<RHI::UploadBuffer<Dante::Utils::DefaultPassConstants>> passCB = nullptr;

		std::unique_ptr<Scene::Model> sponza;
		std::unique_ptr<Scene::Model> helmet;
		std::unique_ptr<Scene::Model> aBeautifulGame;
		std::unique_ptr<Scene::SkySphere> skySphere;
	
		std::unique_ptr<RHI::ShadowMap> shadowMap;

	};
}



#define IMGUI_BEGIN_FRAME \
ImGui_ImplDX12_NewFrame(); \
ImGui_ImplWin32_NewFrame(); \
ImGui::NewFrame(); 

#define IMGUI_END_FRAME \
ImGui::Render(); \
ID3D12DescriptorHeap* heaps[] = { Gfx().ImguiHeap().GetHeap() }; \
cmdList->SetDescriptorHeaps(1, heaps); \
ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList); 