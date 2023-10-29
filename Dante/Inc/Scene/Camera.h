#pragma once
#include "Pch.h"

namespace Dante::Scene
{
	class Camera
	{
	public:
		Camera() = default;
		DISABLE_COPY_AND_MOVE(Camera)
		~Camera() = default;
		void SetView(DirectX::XMVECTOR pos, DirectX::XMVECTOR forward, DirectX::XMVECTOR up);
		void SetProj(float fov, float ar, float nearZ, float farZ);
		DirectX::XMMATRIX GetView();
		DirectX::XMMATRIX GetProj();
		DirectX::XMMATRIX GetViewProj();
		DirectX::XMVECTOR GetPos();
		void Update();

	private:
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR forward;
		DirectX::XMVECTOR up;
		float fov;
		float ar;
		float nearZ;
		float farZ;

		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		DirectX::XMMATRIX viewProj;
		
		bool viewDirty;

	};
}