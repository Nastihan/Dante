#include "Pch.h"
#include "Rendering/Renderer.h"

namespace Dante::Rendering
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
	};

	void Renderer::Init()
	{
		gfx = std::make_unique<Graphics>();
		gfx->Init();
	}

	void Renderer::Render()
	{
		auto cmdListAlloc = gfx->GetCmdAllocator();
		auto cmdList = gfx->GetCmdList();
		auto cmdQueue = gfx->GetCmdQueue();
		Chk(cmdListAlloc->Reset());
		Chk(cmdList->Reset(cmdListAlloc, nullptr));

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		cmdList->RSSetViewports(1, &gfx->GetViewport());
		cmdList->RSSetScissorRects(1, &gfx->GetScissorRect());

		cmdList->ClearRenderTargetView(gfx->CurrentBackBufferView(), DirectX::Colors::Cyan, 0, nullptr);

		cmdList->OMSetRenderTargets(1, &gfx->CurrentBackBufferView(), true, nullptr);

		// draw code


		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ));

		Chk(cmdList->Close());

		ID3D12CommandList* cmdLists[] = {cmdList};
		cmdQueue->ExecuteCommandLists((UINT)std::size(cmdLists), cmdLists);
		
		gfx->Present();

		gfx->FlushCmdQueue();

	}
}