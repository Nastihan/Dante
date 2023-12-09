#include "Pch.h"
#include "Rendering/Renderer.h"
#include "Core/Window.h"
#include "../ThirdParty/imgui/imgui_impl_dx12.h"
#include"../ThirdParty/imgui/imgui_impl_win32.h"
#include "../ThirdParty/imgui/imgui.h"

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
		camera->SetProj(45.0f, Core::Window::Instance().GetAR(), 1.0f, 400.0f);

		defaultPassCB = std::make_unique<RHI::UploadBuffer<Dante::Utils::DefaultPassConstants>>(Gfx(), 1, true);
		shadowPassCB = std::make_unique<RHI::UploadBuffer<Dante::Utils::ShadowPassConstants>>(Gfx(), 1, true);

		sponza = std::make_unique<Scene::Model>(Gfx(),
			"Assests\\Models\\Sponza\\Sponza.glb", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f));
		helmet = std::make_unique<Scene::Model>(Gfx(),
			"Assests\\Models\\DamagedHelmet\\DamagedHelmet.gltf",
			DirectX::XMMatrixTranslation(5.0f, 7.0f, 0.0f) * DirectX::XMMatrixScaling(3.f, 3.0f, 3.0f));
		/*aBeautifulGame = std::make_unique<Scene::Model>(Gfx(),
			"Assests\\Models\\ABeautifulGame\\ABeautifulGame.gltf", 15.0f);*/

		skySphere = std::make_unique<Scene::SkySphere>(Gfx());

		shadowMap = std::make_unique<RHI::ShadowMap>(Gfx(),
			Core::Window::Instance().GetWidth(), Core::Window::Instance().GetHeight());

		
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
		DirectX::XMFLOAT3 lightDirF = { -0.38f, -0.57735f, 0.077735f };

		DirectX::XMVECTOR lightDir = { -0.38f, -0.57735f, 0.077735f };
		auto lightPosition = DirectX::XMVectorScale(lightDir, -2.0f * 270.0f);

		auto forward = DirectX::XMVectorAdd(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), lightPosition);


		auto lightView = DirectX::XMMatrixLookAtLH(lightPosition, 
			DirectX::XMVectorSet(0.01f , 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		auto lightProj = 
			DirectX::XMMatrixPerspectiveFovLH(45.0f, Core::Window::Instance().GetAR(), 1.0f, 700.0f);

		DirectX::XMStoreFloat4x4(&shadowPassConstants.LightView, DirectX::XMMatrixTranspose(lightView));
		DirectX::XMStoreFloat4x4(&shadowPassConstants.LightProj, DirectX::XMMatrixTranspose(lightProj));
		DirectX::XMStoreFloat4x4(&shadowPassConstants.LightViewProj, DirectX::XMMatrixTranspose(lightView * lightProj));
		shadowPassCB->CopyData(0, shadowPassConstants);

		camera->Update(dt);

		DirectX::XMStoreFloat4x4(&defaultPassConstants.View, DirectX::XMMatrixTranspose(lightView));
		DirectX::XMStoreFloat4x4(&defaultPassConstants.Proj, DirectX::XMMatrixTranspose(lightProj));
		DirectX::XMStoreFloat4x4(&defaultPassConstants.ViewProj, DirectX::XMMatrixTranspose(lightView * lightProj));
		DirectX::XMStoreFloat3(&defaultPassConstants.EyePosW, lightPosition);

		defaultPassConstants.lights[0].Strength = { 0.85f, 0.85f, 0.85f };
		defaultPassConstants.lights[0].Direction = { -0.38f, -0.57735f, 0.077735f };
		defaultPassConstants.lights[1].Strength = { 0.95f, 0.95f, 0.95f };
		defaultPassConstants.lights[1].Position = { 0.0f, 18.0f, 2.0f };
		defaultPassCB->CopyData(0, defaultPassConstants);

		
	}

	void Renderer::Render()
	{
		BeginFrame();
		IMGUI_BEGIN_FRAME

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
		cmdList->SetGraphicsRootConstantBufferView(0U, defaultPassCB->Resource()->GetGPUVirtualAddress());

		// Draw Opaque
		cmdList->SetPipelineState(gfx->GetPSO("defaultPSO"));
		sponza->Draw(Gfx());
		helmet->Draw(Gfx());

		// Draw CubeMap
		cmdList->SetPipelineState(gfx->GetPSO("cubeMapPSO"));
		skySphere->Draw(Gfx());

		// GUI
		ShowFpsWindow();
		IMGUI_END_FRAME

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gfx->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ));
		EndFrame();
	}

	void Renderer::ShowFpsWindow()
	{
		ImGui::Begin("FPS");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void Renderer::BeginFrame()
	{
		Chk(gfx->GetCmdAllocator()->Reset());
		Chk(gfx->GetCmdList()->Reset(gfx->GetCmdAllocator(), nullptr));
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