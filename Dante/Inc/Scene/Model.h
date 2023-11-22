#pragma once
#include "Pch.h"
#include "Rendering/RHI/VertexBuffer.h"
#include "Rendering/RHI/IndexBuffer.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Rendering/RHI/Texture.h"
#include "Utils/NastihanMath.h"
#include "../ThirdParty/tinygltf/tiny_gltf.h"


namespace Dante::Scene
{

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 tc;
		DirectX::XMFLOAT3 tangents;
	};

	struct ObjectCB
	{
		DirectX::XMFLOAT4X4 world = Utils::NastihanMath::Identity4x4();
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f, };
		DirectX::XMFLOAT3 FresnelR0 = { 0.2f, 0.2f, 0.2f };
		float Shininess = { 0.1f };
		int albedoMapIndex = { -1};
		int normalMapIndex = { -1};
	};

	class Mesh
	{
	public: 
		Mesh(Rendering::Graphics& gfx, const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& prim,std::string path, float scale);
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
		D3D12_INDEX_BUFFER_VIEW IndexBufferView();
		void Draw(Rendering::Graphics& gfx);
	private:
		std::unique_ptr<Rendering::RHI::UploadBuffer<ObjectCB>> objectCB;
		std::unique_ptr<Dante::Rendering::RHI::VertexBuffer<Vertex>> vertexBuffer;
		std::unique_ptr<Dante::Rendering::RHI::IndexBuffer> indexBuffer;
		std::unique_ptr<Rendering::RHI::Texture> albedoTex;
		std::unique_ptr<Rendering::RHI::Texture> normalTex;
		UINT indexCount;
	};

	class Model
	{
	public:
		Model(Rendering::Graphics& gfx, std::string path, float scale = 1.0f);
		void Draw(Rendering::Graphics& gfx);
	private:
		std::vector<std::unique_ptr<Mesh>> meshes;
	};
}