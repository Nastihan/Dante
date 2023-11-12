#pragma once
#include "Pch.h"
#include "Rendering/RHI/VertexBuffer.h"
#include "Rendering/RHI/IndexBuffer.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Rendering/RHI/Texture.h"
#include "Utils/NastihanMath.h"

namespace Dante::Scene
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 tc;
	};

	struct ObjectCB
	{
		DirectX::XMFLOAT4X4 world = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f, };
		DirectX::XMFLOAT3 FresnelR0 = { 0.2f, 0.2f, 0.2f };
		float Shininess = { 0.4f };
		UINT albedoMapIndex;
	};

	class Model
	{
	public:
		Model(Rendering::Graphics& gfx, std::string path);
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
		D3D12_INDEX_BUFFER_VIEW IndexBufferView();
		void Draw(ID3D12GraphicsCommandList* cmdList);

	private:
		std::unique_ptr<Rendering::RHI::UploadBuffer<ObjectCB>> objectCB;
		std::unique_ptr<Dante::Rendering::RHI::VertexBuffer<Vertex>> vertexBuffer;
		std::unique_ptr<Dante::Rendering::RHI::IndexBuffer> indexBuffer;
		std::unique_ptr<Rendering::RHI::Texture> albedoTex;
		UINT indexCount;
	};
}