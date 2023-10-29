#include "Pch.h"
#include "Camera.h"

namespace Dante::Scene
{

	void Camera::SetView(DirectX::XMVECTOR pos, DirectX::XMVECTOR forward, DirectX::XMVECTOR up)
	{
		this->pos = pos;
		this->forward = forward;
		this->up = up;

		viewDirty = true;
	}

	void Camera::SetProj(float fov, float ar, float nearZ, float farZ)
	{
		this->fov = fov;
		this->ar = ar;
		this->nearZ = nearZ;
		this->farZ = farZ;

		proj = DirectX::XMMatrixPerspectiveFovLH(fov, ar, nearZ, farZ);
		viewDirty = true;
	}

	DirectX::XMMATRIX Camera::GetView()
	{
		return DirectX::XMMatrixTranspose(view);
	}

	DirectX::XMMATRIX Camera::GetProj()
	{
		return DirectX::XMMatrixTranspose(proj);
	}

	DirectX::XMMATRIX Camera::GetViewProj()
	{
		return DirectX::XMMatrixTranspose(view * proj);
	}

	DirectX::XMVECTOR Camera::GetPos()
	{
		return pos;
	}



	void Camera::Update()
	{
		using namespace DirectX;

		if (viewDirty)
		{
			XMVECTOR camPos = pos;
			XMVECTOR camTarget = XMVectorAdd(camPos, forward);
			XMVECTOR camUp = up;

			view = XMMatrixLookAtLH(camPos, camTarget, camUp);

			viewDirty = false;
		}
	}


}