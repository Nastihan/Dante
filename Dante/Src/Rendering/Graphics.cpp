#include "Pch.h"
#include "Rendering\Graphics.h"
#include "Core/Window.h"

namespace Dante::Rendering
{
	void Graphics::Init()
	{
		InitDirect3D();
	}

	void Graphics::OnResize()
	{
		assert(device);
		assert(swapChain);
		assert(mainCmdListAlloc);

		FlushCmdQueue();

		//Chk(cmdList->Reset(mainCmdListAlloc.Get(), nullptr));

		for (auto& b : backBuffers)
		{
			b.Reset();
		}

		Chk(swapChain->ResizeBuffers(
			BACK_BUFFER_COUNT,
			Core::Window::Instance().GetWidth(),
			Core::Window::Instance().GetHeight(),
			backBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		));

		currBackBufferIndex = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			Chk(swapChain->GetBuffer(i, ID(backBuffers.at(i))));
			device->CreateRenderTargetView(backBuffers.at(i).Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}


		FlushCmdQueue();

		screenViewport.TopLeftX = 0;
		screenViewport.TopLeftY = 0;
		screenViewport.Height = FLOAT(Core::Window::Instance().GetHeight());
		screenViewport.Width = FLOAT(Core::Window::Instance().GetWidth());
		screenViewport.MinDepth = 0.0f;
		screenViewport.MaxDepth = 1.0f;

		scissorRect = { 0, 0, LONG(Core::Window::Instance().GetWidth()), LONG(Core::Window::Instance().GetHeight()) };

	}

	void Graphics::InitDirect3D()
	{
		SetupDebugLayer();
		SelectAdapter();
		CreateDevice();
		CreateCommandObjects();
		CreateSwapChain();
		CreateRtvDescriptorHeap();
		OnResize();
	}

	void Graphics::SetupDebugLayer()
	{
#ifdef _DEBUG
		ComPtr<ID3D12Debug6> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);

#endif
	}

	void Graphics::SelectAdapter()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_2;
		UINT createFactoryFlags = 0;

#ifdef _DEBUG
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif 
		Chk(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)));

		size_t maxVideoMemory{};
		ComPtr<IDXGIAdapter1> adapter1;
		ComPtr<IDXGIAdapter4> adapter4;
		for (UINT i = 0; factory->EnumAdapters1(i, adapter1.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			adapter1->GetDesc1(&adapterDesc);

			if (
				(adapterDesc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) &&
				(adapterDesc.DedicatedVideoMemory > maxVideoMemory) &&
				(SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, IID_PPV_ARGS(&device))))
				)
			{
				maxVideoMemory = adapterDesc.DedicatedVideoMemory;
				Chk(adapter1.As(&adapter4));
			}
		}

		adapter = adapter4;

		DXGI_ADAPTER_DESC adapterDesc{};
		adapter->GetDesc(&adapterDesc);
		std::wcout << std::wstring{ adapterDesc.Description } << std::endl;
	}

	void Graphics::CreateDevice()
	{
		Chk(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, ID(device)));
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Graphics::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Chk(device->CreateCommandQueue(&cmdQueueDesc, ID(cmdQueue)));

		Chk(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			ID(mainCmdListAlloc)));

		Chk(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mainCmdListAlloc.Get(),
			nullptr,
			ID(cmdList)
		));

		Chk(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, ID(fence)));

		Chk(cmdList->Close());
	}

	void Graphics::CreateSwapChain()
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = backBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		Chk(device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

		bool tearing = false;
		if (SUCCEEDED (factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing))))
		{
			tearing = true;
		}

		UINT msaaQuality = msQualityLevels.NumQualityLevels;
		assert(msaaQuality > 0 && "Unexpected MSAA quality level.");

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc =
		{
			.Width = Core::Window::Instance().GetWidth(),
			.Height = Core::Window::Instance().GetHeight(),
			.Format = backBufferFormat,
			.Stereo = false,
			.SampleDesc
				{
				.Count = msaaEnabled ? 4U : 1U,
				.Quality = msaaEnabled ? (msaaQuality - 1U) : 0U
				},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = BACK_BUFFER_COUNT,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0U,

		};

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		Chk(factory->CreateSwapChainForHwnd(
			cmdQueue.Get(),
			Core::Window::Instance().GetHWnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		));
		Chk(swapChain1.As(&swapChain));

		currBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	}

	void Graphics::CreateRtvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NodeMask = 0;
		rtvHeapDesc.NumDescriptors = BACK_BUFFER_COUNT;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		Chk(device->CreateDescriptorHeap(&rtvHeapDesc, ID(rtvHeap)));
	}

	void Graphics::FlushCmdQueue()
	{
		fenceVal++;

		Chk(cmdQueue->Signal(fence.Get(), fenceVal));

		if (fence->GetCompletedValue() < fenceVal)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

			Chk(fence->SetEventOnCompletion(fenceVal, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
}