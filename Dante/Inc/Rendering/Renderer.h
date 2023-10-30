#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
#include "Utils/DXUtil.h"
#include "Utils/NastihanMath.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Scene/Camera.h"

namespace Dante::Rendering
{
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
	private:
		void BeginFrame();
		void EndFrame();
	private:
		// TEMP
		void LoadCube();
	private:
		std::unique_ptr<Graphics> gfx{};
		std::unique_ptr<Utils::MeshGeometry> cube;
		std::unique_ptr<Scene::Camera> camera;
		DirectX::XMMATRIX viewProj;
		PassConstants passConstants;
		std::unique_ptr<RHI::UploadBuffer<PassConstants>> passCB = nullptr;

	};
}