#pragma once
#include "Pch.h"
#include "Rendering/RHI/VertexBuffer.h"
#include "Rendering/RHI/IndexBuffer.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Rendering/RHI/CubeMap.h"
#include "Utils/NastihanMath.h"
#include "Utils/GeometryGenerator.h"

namespace Dante::Scene
{
	

	class SkySphere
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
			UINT cubeMapTexIndex = -1;
		};

	public:
		SkySphere(Rendering::Graphics& gfx)
		{
			GeometryGenerator geoGen;
			GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);

			std::vector<SkySphere::Vertex> vertices(sphere.Vertices.size());
			std::vector<USHORT> indices(sphere.Indices32.size());

			for (int i = 0; i < sphere.Vertices.size(); i++)
			{
				vertices[i].pos = sphere.Vertices[i].Position;
				vertices[i].normal = sphere.Vertices[i].Normal;
				vertices[i].tc = sphere.Vertices[i].TexC;
			}

			for (auto& index : sphere.Indices32)
			{
				indices.push_back(index);
			}
			indexCount = UINT(indices.size());

			vertexBuffer = std::make_unique<Dante::Rendering::RHI::VertexBuffer<SkySphere::Vertex>>(
				gfx.GetDevice(), gfx.GetCmdList(), vertices);
			indexBuffer = std::make_unique<Rendering::RHI::IndexBuffer>(gfx.GetDevice(), gfx.GetCmdList(), indices);

			SkySphere::ObjectCB objConstants;
			objConstants.cubeMapTexIndex = gfx.CbvSrvHeap().GetCurrDescriptorIndex();
			cubeMapTex = std::make_unique<Rendering::RHI::CubeMap>(gfx, L"Assests/Textures/snowcube1024.dds");
			objectCB = std::make_unique<Rendering::RHI::UploadBuffer<SkySphere::ObjectCB>>(gfx, 1, true);
			objectCB->CopyData(0, objConstants);
		}

		void Draw(Rendering::Graphics& gfx)
		{
			gfx.GetCmdList()->SetGraphicsRootConstantBufferView(1U, objectCB->Resource()->GetGPUVirtualAddress());
			gfx.GetCmdList()->IASetVertexBuffers(0, 1, &vertexBuffer->View());
			gfx.GetCmdList()->IASetIndexBuffer(&indexBuffer->View());
			gfx.GetCmdList()->DrawIndexedInstanced(indexCount, 1U, 0U, 0U, 0U);
		}
	private:
		std::unique_ptr<Rendering::RHI::UploadBuffer<SkySphere::ObjectCB>> objectCB;
		std::unique_ptr<Dante::Rendering::RHI::VertexBuffer<SkySphere::Vertex>> vertexBuffer;
		std::unique_ptr<Dante::Rendering::RHI::IndexBuffer> indexBuffer;
		UINT indexCount;
		std::unique_ptr<Dante::Rendering::RHI::CubeMap> cubeMapTex;
	};

}