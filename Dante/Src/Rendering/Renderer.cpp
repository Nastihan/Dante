#include "Pch.h"
#include "Rendering/Renderer.h"

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

		LoadTriangle();

		Chk(cmdList->Close());
		ID3D12CommandList* cmdLists[] = { cmdList };
		cmdQueue->ExecuteCommandLists((UINT)std::size(cmdLists), cmdLists);
		gfx->FlushCmdQueue();
	}

	void Renderer::OnResize()
	{
		gfx->OnResize();
	}

	void Renderer::Update()
	{

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

		cmdList->IASetVertexBuffers(0, 1, &triangle->VertexBufferView());
		cmdList->IASetIndexBuffer(&triangle->IndexBufferView());
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawIndexedInstanced(3, 1, 0, 0, 0);




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

	void Renderer::LoadTriangle()
	{
		const std::vector<Vertex> vertices = 
		{
			{ {  0.00f,  0.50f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		};

		const std::vector<USHORT> indices =
		{
			0, 1, 2
		};
		
		triangle = std::make_unique<Utils::MeshGeometry>();

		triangle->vertexBuffer = Utils::DXUtil::CreateDefaultBuffer(gfx->GetDevice(),
			gfx->GetCmdList(), vertices.data(), vertices.size() * sizeof(Vertex));
		triangle->indexBuffer = Utils::DXUtil::CreateDefaultBuffer(gfx->GetDevice(),
			gfx->GetCmdList(), indices.data(), indices.size() * sizeof(USHORT));

		triangle->VertexByteStride = sizeof(Vertex);
		triangle->VertexBufferByteSize = (UINT)vertices.size() * sizeof(Vertex);
		triangle->IndexBufferByteSize = (UINT)indices.size() * sizeof(USHORT);




	}

	
}