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
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	struct Light
	{
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 10.0f;                          // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		float FalloffEnd = 70.0f;                           // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		float SpotPower = 64.0f;                            // spot light only
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float padding;
		DirectX::XMFLOAT4 ambientLight = { 0.10f, 0.10f, 0.10f, 1.0f };
		Light lights[16];
	};
	
	class Renderer
	{
	public:
		Renderer() = default;
		DISABLE_COPY_AND_MOVE(Renderer)
		void Init();
		void OnResize();
		void Update(float dt);
		void Render();
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
		PassConstants passConstants;
		std::unique_ptr<RHI::UploadBuffer<PassConstants>> passCB = nullptr;

		std::unique_ptr<Scene::Model> model;
		std::unique_ptr<Scene::SkySphere> skySphere;
		




	};
}