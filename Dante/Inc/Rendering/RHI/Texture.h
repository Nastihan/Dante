#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
//#include "Utils/DDSTextureLoader.h"
namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture(Graphics& gfx, std::wstring filePath);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}