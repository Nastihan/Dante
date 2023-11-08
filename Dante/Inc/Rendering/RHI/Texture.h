#pragma once
#include "Pch.h"
#include "DescriptorHeap.h"

namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::wstring filePath, DescriptorHeap& heap);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}