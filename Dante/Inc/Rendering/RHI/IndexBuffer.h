#pragma once
#include "Pch.h"
#include <Utils/DXUtil.h>

namespace Dante::Rendering::RHI
{
	class IndexBuffer
	{
	public:
		IndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::vector<USHORT> indices)
		{
			sizeInBytes = (UINT)indices.size() * sizeof(USHORT);
			indexBuffer = Utils::DXUtil::CreateDefaultBuffer(device, cmdList, indices.data(), indices.size() * sizeof(USHORT));
		}
		D3D12_INDEX_BUFFER_VIEW View() const
		{
			assert(indexBuffer.first && indexBuffer.second);
			D3D12_INDEX_BUFFER_VIEW view{};
			view.BufferLocation = indexBuffer.first->GetGPUVirtualAddress();
			view.Format = DXGI_FORMAT_R16_UINT;
			view.SizeInBytes = sizeInBytes;
			return view;
		}
	private:
		std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffer;
		UINT sizeInBytes;
	};
}