#include "Pch.h"
#include "Rendering/Renderer.h"

namespace Dante::Rendering
{

	void Renderer::Init()
	{
		gfx = std::make_unique<Graphics>();
		gfx->Init();
	}

	void Renderer::Render()
	{
		auto cmdListAlloc = gfx->mainCmdListAlloc.Get();
		auto cmdList = gfx->cmdList.Get();
		Chk(cmdListAlloc->Reset());
		Chk(cmdList->Reset(cmdListAlloc, nullptr));

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		cmdList->RSSetViewports(1, &gfx->screenViewport);
		cmdList->RSSetScissorRects(1, &gfx->scissorRect);

		cmdList->ClearRenderTargetView(gfx->CurrentBackBufferView(), DirectX::Colors::Cyan, 0, nullptr);

		cmdList->OMSetRenderTargets(1, &gfx->CurrentBackBufferView(), true, nullptr);

		// draw code



		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ));

		Chk(cmdList->Close());

		ID3D12CommandList* cmdLists[] = {cmdList};
		gfx->cmdQueue->ExecuteCommandLists((UINT)std::size(cmdLists), cmdLists);

		Chk(gfx->swapChain->Present(0, 0));
		gfx->currBackBufferIndex = (gfx->currBackBufferIndex + 1) % gfx->BACK_BUFFER_COUNT;

		gfx->FlushCmdQueue();

	}
}