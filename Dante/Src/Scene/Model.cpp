#include "Pch.h"
#include "Scene/Model.h"



namespace Dante::Scene
{
	Mesh::Mesh(Rendering::Graphics& gfx, const tinygltf::Model& model, UINT meshIndex)
	{

		std::vector<Vertex> vertices;
		std::vector<USHORT> indices;
		
		uint32_t indiciesIndex = model.meshes[0].primitives[meshIndex].indices;
		const tinygltf::Accessor& indexAccessor = model.accessors[indiciesIndex];

		auto positionAttrIndex = model.meshes[0].primitives[meshIndex].attributes.find("POSITION");
		const tinygltf::Accessor& positionAccessor = model.accessors[positionAttrIndex->second];
		const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
		const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];
		int const positionByteStride = positionAccessor.ByteStride(positionBufferView);
		uint8_t const* positions = &positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset];

		auto normalAttrIndex = model.meshes[0].primitives[meshIndex].attributes.find("NORMAL");
		const tinygltf::Accessor& normalAccessor = model.accessors[normalAttrIndex->second];
		const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
		const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
		int const normalByteStride = normalAccessor.ByteStride(normalBufferView);
		uint8_t const* normals = &normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset];

		auto tcAttrIndex = model.meshes[0].primitives[meshIndex].attributes.find("TEXCOORD_0");
		const tinygltf::Accessor& tcAccessor = model.accessors[tcAttrIndex->second];
		const tinygltf::BufferView& tcBufferView = model.bufferViews[tcAccessor.bufferView];
		const tinygltf::Buffer& tcBuffer = model.buffers[tcBufferView.buffer];
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

		const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
		const tinygltf::Buffer& indexBufferBuffer = model.buffers[indexBufferView.buffer];
		int const indexByteStride = indexAccessor.ByteStride(indexBufferView);
		uint8_t const* indicesData = indexBufferBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset;

		for (size_t i = 0; i < indexAccessor.count; i++)
		{
			USHORT index = (USHORT)(reinterpret_cast<uint16_t const*>(indicesData + (i * indexByteStride)))[0];
			indices.push_back(index);
		}
		

		indexCount = (UINT)indices.size();

		vertexBuffer = std::make_unique<Rendering::RHI::VertexBuffer<Vertex>>(gfx.GetDevice(), gfx.GetCmdList(), vertices);
		indexBuffer = std::make_unique<Rendering::RHI::IndexBuffer>(gfx.GetDevice(), gfx.GetCmdList(), indices);

		ObjectCB objCB;
		DirectX::XMStoreFloat4x4(&objCB.world, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
		objCB.albedoMapIndex = gfx.CbvSrvHeap().GetCurrDescriptorIndex();
		//albedoTex = std::make_unique<Rendering::RHI::Texture>(gfx,
		//	L"Assests\\Models\\DamagedHelmet\\Default_albedo.jpg");
		objectCB = std::make_unique<Rendering::RHI::UploadBuffer<ObjectCB>>(gfx, 1, true);
		objectCB->CopyData(0, objCB); 
	}

	D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView()
	{
		return vertexBuffer->View();
	}

	D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView()
	{
		return indexBuffer->View();
	}

	void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->SetGraphicsRootConstantBufferView(1, objectCB->Resource()->GetGPUVirtualAddress());
		cmdList->IASetVertexBuffers(0U, 1U, &vertexBuffer->View());
		cmdList->IASetIndexBuffer(&indexBuffer->View());
		cmdList->DrawIndexedInstanced(indexCount, 1U, 0U, 0U, 0U);
	}


	Model::Model(Rendering::Graphics& gfx, std::string path)
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

		for (UINT i = 0; i < model.meshes[0].primitives.size(); i++)
		{
			meshes.push_back(std::make_unique<Mesh>(gfx, model,i));
		}

		/*
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

		vertexBuffer = std::make_unique<Rendering::RHI::VertexBuffer<Vertex>>(gfx.GetDevice(), gfx.GetCmdList(), vertices);
		indexBuffer = std::make_unique<Rendering::RHI::IndexBuffer>(gfx.GetDevice(), gfx.GetCmdList(), indices);

		ObjectCB objCB;
		DirectX::XMStoreFloat4x4(&objCB.world, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(1.0f,0.0f, 0.0f)));
		objCB.albedoMapIndex = gfx.CbvSrvHeap().GetCurrDescriptorIndex();
		albedoTex = std::make_unique<Rendering::RHI::Texture>(gfx,
			L"Assests\\Models\\DamagedHelmet\\Default_albedo.jpg");
		objectCB = std::make_unique<Rendering::RHI::UploadBuffer<ObjectCB>>(gfx, 1, true);
		objectCB->CopyData(0, objCB); */

	}

	

	void Model::Draw(ID3D12GraphicsCommandList* cmdList)
	{
		for (auto& mesh : meshes)
		{
			mesh->Draw(cmdList);
		}
	}


	

}