#pragma once
#include "Pch.h"

namespace Dante::Rendering::RHI
{
	class Texture
	{
	public:
		Texture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::wstring filePath, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource{};
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource{};
	};
}