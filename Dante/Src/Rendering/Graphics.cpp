#include "Pch.h"
#include "Rendering\Graphics.h"
#include "Core/Window.h"

namespace Dante::Rendering
{
	///////////// Initialization
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
		Chk(cmdList->Reset(mainCmdListAlloc.Get(), nullptr));

		for (auto& b : backBuffers)
		{
			b.Reset();
		}
		depthStencilBuffer.Reset();

		// back buffer stuff
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChain->GetDesc(&swapChainDesc);
		Chk(swapChain->ResizeBuffers(
			BACK_BUFFER_COUNT,
			Core::Window::Instance().GetWidth(),
			Core::Window::Instance().GetHeight(),
			backBufferFormat,
			swapChainDesc.Flags
		));
		currBackBufferIndex = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			Chk(swapChain->GetBuffer(i, ID(backBuffers.at(i))));
			device->CreateRenderTargetView(backBuffers.at(i).Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}

		// depth stencil stuff
		D3D12_RESOURCE_DESC dsDesc{};
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Alignment = 0;
		dsDesc.Width = Core::Window::Instance().GetWidth();
		dsDesc.Height = Core::Window::Instance().GetHeight();
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;
		dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		dsDesc.SampleDesc.Count = msaaEnabled ? 4 : 1;
		dsDesc.SampleDesc.Quality = msaaEnabled ? (msaaQuality - 1) : 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		Chk(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&dsDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&CD3DX12_CLEAR_VALUE(depthStencilFormat, 1.0f, 0U),
			ID(depthStencilBuffer)
		));
		// ds view
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = depthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, DepthStencilView());
	
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE
		));
		
		// execute
		Chk(cmdList->Close());
		ID3D12CommandList* cmdLists[] = { cmdList.Get()};
		cmdQueue->ExecuteCommandLists(1, cmdLists);
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
		CreateRtvAndDsvDescriptorHeap();
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

		
		BOOL tearing = FALSE;
		if (SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearing, sizeof(tearing))))
		{
			tearing = TRUE;
		}

		msaaQuality = msQualityLevels.NumQualityLevels;
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

	void Graphics::CreateRtvAndDsvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NodeMask = 0;
		rtvHeapDesc.NumDescriptors = BACK_BUFFER_COUNT;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		Chk(device->CreateDescriptorHeap(&rtvHeapDesc, ID(rtvHeap)));

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NodeMask = 0;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		Chk(device->CreateDescriptorHeap(&dsvHeapDesc, ID(dsvHeap)));
	}

	void Graphics::Present()
	{
		Chk(swapChain->Present(0, 0));
		currBackBufferIndex = (currBackBufferIndex + 1) % BACK_BUFFER_COUNT;
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

	void Graphics::BuildPSOs()
	{
		
	}

	void Graphics::BuildRootSigs()
	{
	}

	void Graphics::BuildShaders()
	{
	}

	//////////////////////// Getters
	ID3D12Device* Graphics::GetDevice()
	{
		return device.Get();
	}

	ID3D12CommandAllocator* Graphics::GetCmdAllocator()
	{
		return mainCmdListAlloc.Get();
	}

	ID3D12GraphicsCommandList* Graphics::GetCmdList()
	{
		return cmdList.Get();
	}

	ID3D12CommandQueue* Graphics::GetCmdQueue()
	{
		return cmdQueue.Get();
	}

	D3D12_VIEWPORT Graphics::GetViewport()
	{
		return screenViewport;
	}

	D3D12_RECT Graphics::GetScissorRect()
	{
		return scissorRect;
	}

	ID3D12Resource* Graphics::CurrentBackBuffer()
	{
		return backBuffers.at(currBackBufferIndex).Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::CurrentBackBufferView()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE{
			rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			int(currBackBufferIndex),
			rtvDescriptorSize
		};
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DepthStencilView()
	{
		return dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	

}