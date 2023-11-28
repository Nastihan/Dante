#include "Pch.h"
#include "Scene/SkySphere.h"

namespace Dante::Scene
{
	SkySphere::SkySphere(Rendering::Graphics& gfx)
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
		cubeMapTex = std::make_unique<Rendering::RHI::Texture>();
		cubeMapTex->CreateCubeDDS(gfx, L"Assests/Textures/snowcube1024.dds");
		objectCB = std::make_unique<Rendering::RHI::UploadBuffer<SkySphere::ObjectCB>>(gfx, 1, true);
		objectCB->CopyData(0, objConstants);
		
	}

	void SkySphere::Draw(Rendering::Graphics& gfx)
	{
		gfx.GetCmdList()->SetGraphicsRootConstantBufferView(1U, objectCB->Resource()->GetGPUVirtualAddress());
		gfx.GetCmdList()->IASetVertexBuffers(0, 1, &vertexBuffer->View());
		gfx.GetCmdList()->IASetIndexBuffer(&indexBuffer->View());
		gfx.GetCmdList()->DrawIndexedInstanced(indexCount, 1U, 0U, 0U, 0U);
	}


}