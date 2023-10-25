#pragma once
#include "Pch.h"

namespace Dante::Utils
{
	class DXUtil
	{
	public:
		static std::pair<ComPtr<ID3D12Resource>, ComPtr<ID3D12Resource>> 
			CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
				const void* data, UINT64 byteSize);


	};

	struct MeshGeometry
	{
		std::string name;

		std::pair<ComPtr<ID3D12Resource>, ComPtr<ID3D12Resource>> vertexBuffer;
		std::pair<ComPtr<ID3D12Resource>, ComPtr<ID3D12Resource>> indexBuffer;
		

		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = vertexBuffer.first->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = indexBuffer.first->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}
	};

}