#pragma once
#include "Pch.h"
#include "Rendering/Graphics.h"

namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture() =  default;
		DISABLE_COPY_AND_MOVE(Texture)
		void Create2D(Graphics& gfx, std::wstring filePath);
		void CreateCubeDDS(Graphics& gfx, std::wstring filePath);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}