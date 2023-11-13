#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"
namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture(Graphics& gfx, std::wstring filePath);
		Texture(Graphics& gfx, const void* data);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}