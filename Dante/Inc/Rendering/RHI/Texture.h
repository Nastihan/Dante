#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture(Graphics& gfx, std::wstring filePath, bool isCubeMap = false);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}