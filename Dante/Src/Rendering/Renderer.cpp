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
		camera->SetView({ 0.0f, 8.0f, -6.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
		camera->SetProj(45.0f, Core::Window::Instance().GetAR(), 1.0f, 300.0f);

		passCB = std::make_unique<RHI::UploadBuffer<PassConstants>>(Gfx(), 1, true);

		model = std::make_unique<Scene::Model>(Gfx(),
			"Assests\\Models\\Sponza\\Sponza.gltf");

		skySphere = std::make_unique<Scene::SkySphere>(Gfx());

		
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

		passConstants.lights[0].Strength = { 0.35f, 0.35f, 0.35f };
		passConstants.lights[0].Direction = { -0.24f, -0.57735f, 0.57735f };
		passConstants.lights[1].Strength = { 0.95f, 0.95f, 0.95f };
		passConstants.lights[1].Position = { 0.0f, 18.0f, 2.0f };


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

		cmdList->ClearRenderTargetView(gfx->CurrentBackBufferView(), DirectX::Colors::DarkSlateGray, 0, nullptr);
		cmdList->ClearDepthStencilView(gfx->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0U, 0U, nullptr);

		cmdList->OMSetRenderTargets(1, &gfx->CurrentBackBufferView(), true, &gfx->DepthStencilView());

		ID3D12DescriptorHeap* descHeaps[] = { Gfx().CbvSrvHeap().GetHeap() };
		cmdList->SetDescriptorHeaps(1, descHeaps);
		cmdList->SetGraphicsRootSignature(gfx->GetRootSig("defaultRS"));
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set PassCB
		cmdList->SetGraphicsRootConstantBufferView(0U, passCB->Resource()->GetGPUVirtualAddress());

		// Draw Opaque
		model->Draw(Gfx());

		// Draw CubeMap
		cmdList->SetPipelineState(gfx->GetPSO("cubeMapPSO"));
		skySphere->Draw(Gfx());




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