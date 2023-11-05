#pragma once
#include "Pch.h"
#include "Rendering/RHI/VertexBuffer.h"
#include "Rendering/RHI/IndexBuffer.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Utils/NastihanMath.h"

namespace Dante::Scene
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};

	struct ObjectCB
	{
		DirectX::XMFLOAT4X4 world = Utils::NastihanMath::Identity4x4();

	};


	class Model
	{
	public:
		Model(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string path);
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
		D3D12_INDEX_BUFFER_VIEW IndexBufferView();
		void Draw(ID3D12GraphicsCommandList* cmdList);


	private:
		std::unique_ptr < Rendering::RHI::UploadBuffer<ObjectCB>> objectCB;
		std::unique_ptr<Dante::Rendering::RHI::VertexBuffer<Vertex>> vertexBuffer;
		std::unique_ptr<Dante::Rendering::RHI::IndexBuffer> indexBuffer;
		UINT indexCount;
	};
}