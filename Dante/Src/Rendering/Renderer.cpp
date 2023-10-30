#include "Pch.h"
#include "Rendering/Renderer.h"
#include "Core/Window.h"
namespace Dante::Rendering
{


	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	void Renderer::Init()
	{
		gfx = std::make_unique<Graphics>();
		gfx->Init();

		auto cmdListAlloc = gfx->GetCmdAllocator();
		auto cmdList = gfx->GetCmdList();
		auto cmdQueue = gfx->GetCmdQueue();
		Chk(cmdListAlloc->Reset());
		Chk(cmdList->Reset(cmdListAlloc, nullptr));

		gfx->Load();
		LoadCube();

		camera = std::make_unique<Scene::Camera>();
		camera->SetView({ 0.0f, 0.0f, -6.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
		camera->SetProj(45.0f, Core::Window::Instance().GetAR(), 1.0f, 100.0f);

		passCB = std::make_unique<RHI::UploadBuffer<PassConstants>>(gfx->GetDevice(), 1, true);
		
		Chk(cmdList->Close());
		ID3D12CommandList* cmdLists[] = { cmdList };
		cmdQueue->ExecuteCommandLists((UINT)std::size(cmdLists), cmdLists);
		gfx->FlushCmdQueue();


	}

	void Renderer::OnResize()
	{
		gfx->OnResize();
	}

	void Renderer::Update(float dt)
	{
		camera->Update(dt);

		DirectX::XMStoreFloat4x4(&passConstants.View, camera->GetView());
		DirectX::XMStoreFloat4x4(&passConstants.Proj, camera->GetProj());
		DirectX::XMStoreFloat4x4(&passConstants.ViewProj, camera->GetViewProj());
		DirectX::XMStoreFloat3(&passConstants.EyePosW, camera->GetPos());

		passCB->CopyData(0, passConstants);
	}

	void Renderer::Render()
	{
		BeginFrame();

		auto cmdList = gfx->GetCmdList();

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		cmdList->RSSetViewports(1, &gfx->GetViewport());
		cmdList->RSSetScissorRects(1, &gfx->GetScissorRect());

		cmdList->ClearRenderTargetView(gfx->CurrentBackBufferView(), DirectX::Colors::Cyan, 0, nullptr);
		cmdList->ClearDepthStencilView(gfx->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0U, 0U, nullptr);

		cmdList->OMSetRenderTargets(1, &gfx->CurrentBackBufferView(), true, &gfx->DepthStencilView());

		// draw code
		cmdList->SetGraphicsRootSignature(gfx->GetRootSig("defaultRS"));
		cmdList->SetGraphicsRootConstantBufferView(0U, passCB->Resource()->GetGPUVirtualAddress());

		cmdList->IASetVertexBuffers(0, 1, &cube->VertexBufferView());
		cmdList->IASetIndexBuffer(&cube->IndexBufferView());
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawIndexedInstanced(cube->DrawArgs["triangle"].IndexCount,
			1, cube->DrawArgs["triangle"].StartIndexLocation,
			cube->DrawArgs["triangle"].BaseVertexLocation, 0);


		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ));

		EndFrame();
	}

	void Renderer::BeginFrame()
	{
		Chk(gfx->GetCmdAllocator()->Reset());
		Chk(gfx->GetCmdList()->Reset(gfx->GetCmdAllocator(), gfx->GetPSO("defaultPSO")));
	}

	void Renderer::EndFrame()
	{
		Chk(gfx->GetCmdList()->Close());
		ID3D12CommandList* cmdLists[] = { gfx->GetCmdList() };
		gfx->GetCmdQueue()->ExecuteCommandLists((UINT)std::size(cmdLists), cmdLists);
		gfx->Present();
		gfx->FlushCmdQueue();
	}

	void Renderer::LoadCube()
	{
		const std::vector<Vertex> vertices =
		{
			// Front face
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-left (0)
			{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-right (1)
			{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Top-left (2)
			{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-right (3)
			// Back face						   , 0.0f, 1.0f
			{ { -1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } },  // Bottom-left (4)
			{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },  // Bottom-right (5)
			{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },  // Top-left (6)
			{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } },  // Top-right (7)
			// Left face						   , 0.0f, 1.0f
			{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-left (8)
			{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-left (9)
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-right (10)
			{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Top-right (11)
			// Right face						   , 0.0f, 1.0f
			{ { 1.0f, -1.0f,  -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-left (12)
			{ { 1.0f,  1.0f,  -1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Top-left (13)
			{ { 1.0f, -1.0f,   1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-right (14)
			{ { 1.0f,  1.0f,   1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-right (15)

			// Bottom face						   , 0.0f, 1.0f
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-left (20)
			{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-right (21)
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Top-left (22)
			{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-right (23)
			// Top face							   , 0.0f, 1.0f
			{ { -1.0f, 1.0f,  -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-left (16)
			{ {  1.0f, 1.0f,  -1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom-right (17)
			{ { -1.0f, 1.0f,   1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Top-left (18)
			{ {  1.0f, 1.0f,   1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-right (19)
		};
		const std::vector<USHORT> indices =
		{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
		};
		
		cube = std::make_unique<Utils::MeshGeometry>();

		cube->vertexBuffer = Utils::DXUtil::CreateDefaultBuffer(gfx->GetDevice(),
			gfx->GetCmdList(), vertices.data(), vertices.size() * sizeof(Vertex));
		cube->indexBuffer = Utils::DXUtil::CreateDefaultBuffer(gfx->GetDevice(),
			gfx->GetCmdList(), indices.data(), indices.size() * sizeof(USHORT));

		cube->VertexByteStride = sizeof(Vertex);
		cube->VertexBufferByteSize = (UINT)vertices.size() * sizeof(Vertex);
		cube->IndexBufferByteSize = (UINT)indices.size() * sizeof(USHORT);
		
		cube->DrawArgs["triangle"].IndexCount = (UINT)indices.size();
		cube->DrawArgs["triangle"].StartIndexLocation = 0;
	}

	
}