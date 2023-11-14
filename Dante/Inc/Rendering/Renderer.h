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
		float FalloffStart = 5.0f;                          // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		float FalloffEnd = 50.0f;                           // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		float SpotPower = 64.0f;                            // spot light only
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = Utils::NastihanMath::Identity4x4();
		//DirectX::XMFLOAT4X4 InvView = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = Utils::NastihanMath::Identity4x4();
		//DirectX::XMFLOAT4X4 InvProj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = Utils::NastihanMath::Identity4x4();
		//DirectX::XMFLOAT4X4 InvViewProj = Utils::NastihanMath::Identity4x4();
		//DirectX::XMFLOAT4X4 ShadowTransform = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float padding;

		DirectX::XMFLOAT4 ambientLight = { 0.10f, 0.10f, 0.10f, 1.0f };

		Light lights[16];
		//float cbPerObjectPad1 = 0.0f;
		//DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		//DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		//float NearZ = 0.0f;
		//float FarZ = 0.0f;
		//float TotalTime = 0.0f;
		//float DeltaTime = 0.0f;
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
		DirectX::XMMATRIX viewProj;
		PassConstants passConstants;
		std::unique_ptr<RHI::UploadBuffer<PassConstants>> passCB = nullptr;
		std::unique_ptr<Scene::Model> model;



	};
}