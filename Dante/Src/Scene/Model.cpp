#include "Pch.h"
#include "Scene/Model.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP20
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "../ThirdParty/tinygltf/tiny_gltf.h"

namespace Dante::Scene
{
	Model::Model(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::string path, Rendering::RHI::DescriptorHeap& heap)
	{
		std::string warning{};
		std::string error{};

		tinygltf::TinyGLTF context;
		tinygltf::Model model;

		if (!context.LoadASCIIFromFile(&model, &error, &warning, path))
		{
			if (!error.empty())
			{
				OutputDebugStringA(error.c_str());
			}

			if (!warning.empty())
			{
				OutputDebugStringA(warning.c_str());
			}
		}

		std::vector<Vertex> vertices;
		std::vector<USHORT> indices;

		tinygltf::Scene& scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			tinygltf::Node& node = model.nodes[scene.nodes[i]];
			if (node.mesh < 0 || node.mesh > model.meshes.size()) continue;

			tinygltf::Mesh& mesh = model.meshes[node.mesh];
			for (size_t i = 0; i < mesh.primitives.size(); i++)
			{
				tinygltf::Primitive primitive = mesh.primitives[i];
				tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

				tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes["POSITION"]];
				tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
				tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
				int const positionByteStride = positionAccessor.ByteStride(positionBufferView);
				uint8_t const* positions = &positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset];

				tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
				tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
				tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
				int const normalByteStride = normalAccessor.ByteStride(normalBufferView);
				uint8_t const* normals = &normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset];

				tinygltf::Accessor& tcAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
				tinygltf::BufferView& tcBufferView = model.bufferViews[tcAccessor.bufferView];
				tinygltf::Buffer& tcBuffer = model.buffers[tcBufferView.buffer];
				int const tcByteStride = tcAccessor.ByteStride(tcBufferView);
				uint8_t const* texcoords = &tcBuffer.data[tcBufferView.byteOffset + tcAccessor.byteOffset];

				for (size_t i = 0; i < positionAccessor.count; i++)
				{
					Vertex vertex{};
					vertex.pos.x = (reinterpret_cast<float const*>(positions + (i * positionByteStride)))[0];
					vertex.pos.y = (reinterpret_cast<float const*>(positions + (i * positionByteStride)))[1];
					vertex.pos.z = (reinterpret_cast<float const*>(positions + (i * positionByteStride)))[2];

					vertex.normal.x = (reinterpret_cast<float const*>(normals + (i * normalByteStride)))[0];
					vertex.normal.y = (reinterpret_cast<float const*>(normals + (i * normalByteStride)))[1];
					vertex.normal.z = (reinterpret_cast<float const*>(normals + (i * normalByteStride)))[2];

					vertex.tc.x = (reinterpret_cast<float const*>(texcoords + (i * tcByteStride)))[0];
					vertex.tc.y = (reinterpret_cast<float const*>(texcoords + (i * tcByteStride)))[1];
					//vertex.tc.y = 1.0f - vertex.tc.y;

					vertices.push_back(vertex);
				}

				tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
				tinygltf::Buffer& indexBufferBuffer = model.buffers[indexBufferView.buffer];
				int const indexByteStride = indexAccessor.ByteStride(indexBufferView);
				uint8_t const* indicesData = indexBufferBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset;

				for (size_t i = 0; i < indexAccessor.count; i++)
				{
					USHORT index = (USHORT)(reinterpret_cast<uint16_t const*>(indicesData + (i * indexByteStride)))[0];
					indices.push_back(index);
				}
			}
		}

		indexCount = (UINT)indices.size();

		vertexBuffer = std::make_unique<Rendering::RHI::VertexBuffer<Vertex>>(device, cmdList, vertices);
		indexBuffer = std::make_unique<Rendering::RHI::IndexBuffer>(device, cmdList, indices);

		

		ObjectCB objCB;
		DirectX::XMStoreFloat4x4(&objCB.world, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(1.0f,0.0f, 0.0f)));
		objCB.albedoMapIndex = 0U;
		objectCB = std::make_unique<Rendering::RHI::UploadBuffer<ObjectCB>>(device, 1, true);
		objectCB->CopyData(0, objCB);

		albedoTex = std::make_unique<Rendering::RHI::Texture>(device, cmdList,
			L"Assests\\Models\\DamagedHelmet\\Default_albedo.jpg", heap);
	}

	D3D12_VERTEX_BUFFER_VIEW Model::VertexBufferView()
	{
		return vertexBuffer->View();
	}

	D3D12_INDEX_BUFFER_VIEW Model::IndexBufferView()
	{
		return indexBuffer->View();
	}

	void Model::Draw(ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->SetGraphicsRootConstantBufferView(1, objectCB->Resource()->GetGPUVirtualAddress());
		cmdList->IASetVertexBuffers(0U, 1U, &vertexBuffer->View());
		cmdList->IASetIndexBuffer(&indexBuffer->View());
		cmdList->DrawIndexedInstanced(indexCount, 1U, 0U, 0U, 0U);
	}


}