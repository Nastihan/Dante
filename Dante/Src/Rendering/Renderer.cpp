#include "Pch.h"
#include "Rendering/Renderer.h"
#include "Core/Window.h"
namespace Dante::Rendering
{
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
		
		camera = std::make_unique<Scene::Camera>();
		camera->SetView({ 0.0f, 0.0f, -6.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
		camera->SetProj(45.0f, Core::Window::Instance().GetAR(), 1.0f, 100.0f);

		passCB = std::make_unique<RHI::UploadBuffer<PassConstants>>(gfx->GetDevice(), 1, true);

		model = std::make_unique<Scene::Model>(gfx->GetDevice(), gfx->GetCmdList(),
			"Assests\\Models\\DamagedHelmet\\DamagedHelmet.gltf", 
			gfx->cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
		
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
		ID3D12DescriptorHeap* descHeaps[] = { gfx->cbvSrvUavHeap.Get()};
		cmdList->SetDescriptorHeaps(1, descHeaps);

		cmdList->SetGraphicsRootSignature(gfx->GetRootSig("defaultRS"));
		cmdList->SetGraphicsRootConstantBufferView(0U, passCB->Resource()->GetGPUVirtualAddress());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		model->Draw(gfx->GetCmdList(), gfx->cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());


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

	
}