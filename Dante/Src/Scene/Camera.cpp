#include "Pch.h"
#include "Scene/Camera.h"
#include <Utils/NastihanMath.h>

namespace Dante::Scene
{

	void Camera::SetView(DirectX::XMVECTOR pos, DirectX::XMVECTOR forward, DirectX::XMVECTOR up)
	{
		DirectX::XMStoreFloat3(&this->pos, pos);
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
		
		return DirectX::XMLoadFloat3(&pos);
	}

	void Camera::Translate(DirectX::XMFLOAT3 translation)
	{
		namespace dx = DirectX;
		dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
		));
		pos = {
			pos.x + translation.x,
			pos.y + translation.y,
			pos.z + translation.z
		};

		viewDirty = true;
	}

	void Camera::Rotate(float dx, float dy)
	{
		yaw = Utils::NastihanMath::wrap_angle(yaw + dx );	
		pitch = std::clamp(pitch + dy , 0.999f * -Dante::Utils::NastihanMath::PI / 2.0f, 0.999f * Dante::Utils::NastihanMath::PI / 2.0f);
		viewDirty = true;
	}

	void Camera::Update(float dt)
	{
		float moveSpeed = dt * speed;

		if (GetAsyncKeyState('A') & 0x8000)
			Translate({ -moveSpeed, 0.0f, 0.0f });

		if (GetAsyncKeyState('D') & 0x8000)
			Translate({ moveSpeed, 0.0f, 0.0f });

		if (GetAsyncKeyState('W') & 0x8000)
			Translate({ 0.0f, 0.0f, moveSpeed });

		if (GetAsyncKeyState('S') & 0x8000)
			Translate({ 0.0f, 0.0f, -moveSpeed });

		if (GetAsyncKeyState('R') & 0x8000)
			Translate({ 0.0f, moveSpeed, 0.0f });

		if (GetAsyncKeyState('F') & 0x8000)
			Translate({ 0.0f, -moveSpeed, 0.0f });

		// [TODO] handle rotation in a better way
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			Rotate(0.0f, -moveSpeed);

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			Rotate(0.0f, moveSpeed);
		
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			Rotate(moveSpeed, 0.0f);

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			Rotate(-moveSpeed, 0.0f);

		if (viewDirty)
		{
			using namespace DirectX;

			XMVECTOR camPos = XMLoadFloat3(&pos);

			XMVECTOR camTarget = XMVectorAdd(camPos, XMVector3Transform(forward, XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)));
			XMVECTOR camUp = up;

			view = XMMatrixLookAtLH(camPos, camTarget, camUp);

			viewDirty = false;
		}
	}


}