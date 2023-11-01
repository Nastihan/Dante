#pragma once
#include "Pch.h"
#include <Utils/DXUtil.h>

namespace Dante::Rendering::RHI
{
	template <typename V>
	class VertexBuffer
	{
	public:
		VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::vector<V> vertices)
		{
			strideInBytes = sizeof(V);
			sizeInBytes = (UINT)vertices.size() * sizeof(V);
			vertexBuffer = Utils::DXUtil::CreateDefaultBuffer(device, cmdList, vertices.data(), vertices.size() * sizeof(V));
		}
		D3D12_VERTEX_BUFFER_VIEW View() const
		{
			D3D12_VERTEX_BUFFER_VIEW view{};
			view.BufferLocation = vertexBuffer.first->GetGPUVirtualAddress();
			view.StrideInBytes = strideInBytes;
			view.SizeInBytes = sizeInBytes;
			return view;
		}
	private:
		std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffer;
		UINT sizeInBytes;
		UINT strideInBytes;
		
	};
}