#pragma once
#include "Pch.h"
#include "Utils/NastihanMath.h"

namespace Dante::Utils
{
	class DXUtil
	{
	public:
		static std::pair<ComPtr<ID3D12Resource>, ComPtr<ID3D12Resource>> 
			CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
				const void* data, UINT64 byteSize);
		static UINT CalcCBufferByteSize(UINT byteSize);

	};

	struct SubmeshGeometry
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;
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

		std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

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

	struct Light
	{
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 10.0f;                          // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		float FalloffEnd = 70.0f;                           // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		float SpotPower = 64.0f;                            // spot light only
	};

	struct DefaultPassConstants
	{
		DirectX::XMFLOAT4X4 View = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float padding;
		DirectX::XMFLOAT4 ambientLight = { 0.10f, 0.10f, 0.10f, 1.0f };
		Light lights[16];
	};

	struct ShadowPassConstants
	{
		DirectX::XMFLOAT4X4 LightView = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 LightProj = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4X4 LightViewProj = Utils::NastihanMath::Identity4x4();
		//DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	};
}