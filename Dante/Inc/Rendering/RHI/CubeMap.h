#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"

namespace Dante::Rendering::RHI
{
	class CubeMap
	{
	public:
		CubeMap(Rendering::Graphics& gfx, std::wstring filePath);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
	};
}