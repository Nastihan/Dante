#pragma once
#include <Pch.h>
#include <Rendering/Graphics.h>

namespace Dante::Rendering::RHI
{
	class ShadowMap
	{
	public:
		ShadowMap(Rendering::Graphics& gfx, UINT width, UINT height);
		DISABLE_COPY_AND_MOVE(ShadowMap)

	private:
		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorRect;

		UINT width = 0;
		UINT height = 0;
		DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS;

		Microsoft::WRL::ComPtr<ID3D12Resource> shadowMap = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDsvHandle;
		UINT shadowMapIndex;
	};

}