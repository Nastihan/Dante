#include "Pch.h"
#include "Rendering\Graphics.h"
#include "Core/Window.h"
#include "../ThirdParty/imgui/imgui_impl_dx12.h"

namespace Dante::Rendering
{
	Graphics::~Graphics()
	{
		if (device) FlushCmdQueue();
		ImGui_ImplDX12_Shutdown();
	}
	///////////////////////// Initialization
	void Graphics::Init()
	{
		InitDirect3D();
	}

	void Graphics::OnResize()
	{
		Resize();
	}

	void Graphics::Resize()
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
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetHandleForStart().cpuHandle);
		for (UINT i = 0; i < BACK_BUFFER_COUNT; i++)
		{
			auto cpuHandle = rtvHeap->GetHandleForStart().cpuHandle;
			cpuHandle.Offset(i, rtvDescriptorSize);
			Chk(swapChain->GetBuffer(i, ID(backBuffers.at(i))));
			device->CreateRenderTargetView(backBuffers.at(i).Get(), nullptr, cpuHandle);
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
		dsvHeap->OffsetCurrHandle();
	
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
		CreateCbvSrvUavDescriptorHeap();
		OnResize();
		InitImgui();
	}

	void Graphics::SetupDebugLayer()
	{
#ifdef _DEBUG
		ComPtr<ID3D12Debug6> debugController;
		Chk(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
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
		rtvHeap = std::make_unique<RHI::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BACK_BUFFER_COUNT);

		dsvHeap = std::make_unique<RHI::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 4);
	}

	void Graphics::CreateCbvSrvUavDescriptorHeap()
	{
		cbvHeap = std::make_unique<RHI::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000);
		
	}

	void Graphics::InitImgui()
	{
		imguiHeap = std::make_unique<RHI::DescriptorHeap>(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		ImGui_ImplDX12_Init(device.Get(), BACK_BUFFER_COUNT, backBufferFormat, imguiHeap->GetHeap(),
			imguiHeap->GetHandleForStart().cpuHandle, imguiHeap->GetHandleForStart().gpuHandle);
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
	
	/////////////////////////// Load
	void Graphics::Load()
	{
		BuildRootSigs();
		BuildShaders();
		BuildPSOs();
	}

	void Graphics::BuildRootSigs()
	{
		BuildStaticSamplers(samplers);

		CD3DX12_DESCRIPTOR_RANGE1 descRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1U, 0U };

		CD3DX12_ROOT_PARAMETER1 rootParams[2]{};
		rootParams[0].InitAsConstantBufferView(0U, 0U, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParams[1].InitAsConstantBufferView(1U, 0U, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
		rootSigDesc.Init_1_1((UINT)std::size(rootParams), rootParams, (UINT)samplers.size(), samplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);

		ComPtr<ID3DBlob> serializedRootSig=nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
		if (errorBlob )
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		Chk(hr);

		Chk(device->CreateRootSignature(
			0, serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			ID(rootSignatures["defaultRS"])
		));

	}

	void Graphics::BuildShaders()
	{
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\DefaultVS.cso", shaders["defaultVS"].GetAddressOf()));
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\DefaultPS.cso", shaders["defaultPS"].GetAddressOf()));
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\CubeMapVS.cso", shaders["cubeMapVS"].GetAddressOf()));
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\CubeMapPS.cso", shaders["cubeMapPS"].GetAddressOf()));
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\ShadowMapVS.cso", shaders["shadowMapVS"].GetAddressOf()));
		Chk(D3DReadFileToBlob(L"Shaders\\ShaderBins\\ShadowMapPS.cso", shaders["shadowMapPS"].GetAddressOf()));
	}

	void Graphics::BuildStaticSamplers(std::array<CD3DX12_STATIC_SAMPLER_DESC, 6>& samplers)
	{
		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			16U);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			16U);                                // maxAnisotropy

		samplers[0] = pointWrap;
		samplers[1] = pointClamp;
		samplers[2] = linearWrap;
		samplers[3] = linearClamp;
		samplers[4] = anisotropicWrap;
		samplers[5] = anisotropicClamp;

	}

	void Graphics::BuildPSOs()
	{
		inputLayout =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc{};
		opaquePsoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
		opaquePsoDesc.pRootSignature = rootSignatures["defaultRS"].Get();
		opaquePsoDesc.VS = CD3DX12_SHADER_BYTECODE(shaders["defaultVS"].Get());
		opaquePsoDesc.PS = CD3DX12_SHADER_BYTECODE(shaders["defaultPS"].Get());
		opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaquePsoDesc.SampleMask = UINT_MAX;
		opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaquePsoDesc.NumRenderTargets = 1;
		opaquePsoDesc.RTVFormats[0] = backBufferFormat;
		opaquePsoDesc.SampleDesc.Count = msaaEnabled ? 4 : 1;
		opaquePsoDesc.SampleDesc.Quality = msaaEnabled ? (msaaQuality - 1) : 0;
		opaquePsoDesc.DSVFormat = depthStencilFormat;

		Chk(device->CreateGraphicsPipelineState(&opaquePsoDesc, ID(pSOs["defaultPSO"])));

		D3D12_GRAPHICS_PIPELINE_STATE_DESC cubeMapPsoDesc = opaquePsoDesc;
		cubeMapPsoDesc.pRootSignature = rootSignatures["defaultRS"].Get();
		cubeMapPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		cubeMapPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		cubeMapPsoDesc.VS = CD3DX12_SHADER_BYTECODE(shaders["cubeMapVS"].Get());
		cubeMapPsoDesc.PS = CD3DX12_SHADER_BYTECODE(shaders["cubeMapPS"].Get());

		Chk(device->CreateGraphicsPipelineState(&cubeMapPsoDesc, ID(pSOs["cubeMapPSO"])));

		D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowMapPsoDesc = opaquePsoDesc;
		shadowMapPsoDesc.RasterizerState.DepthBias = 100000;
		shadowMapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		shadowMapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		shadowMapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		shadowMapPsoDesc.NumRenderTargets = 0;
		shadowMapPsoDesc.VS = CD3DX12_SHADER_BYTECODE(shaders["shadowMapVS"].Get());
		shadowMapPsoDesc.PS = CD3DX12_SHADER_BYTECODE(shaders["shadowMapPS"].Get());
		Chk(device->CreateGraphicsPipelineState(&shadowMapPsoDesc, ID(pSOs["shadowMapPSO"])));

	}

	/////////////////////////// Getters
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

	ID3D12PipelineState* Graphics::GetPSO(std::string name)
	{
		return pSOs[name].Get();
	}

	ID3D12RootSignature* Graphics::GetRootSig(std::string name)
	{
		return rootSignatures[name].Get();
	}

	RHI::DescriptorHeap& Graphics::CbvSrvHeap()
	{
		return *cbvHeap;
	}

	RHI::DescriptorHeap& Graphics::DsvHeap()
	{
		return *dsvHeap;
	}

	RHI::DescriptorHeap& Graphics::ImguiHeap()
	{
		return *imguiHeap;
	}

	/////////////////////////// Utils
	ID3D12Resource* Graphics::CurrentBackBuffer()
	{
		return backBuffers.at(currBackBufferIndex).Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::CurrentBackBufferView()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE{
					rtvHeap->GetHandleForStart().cpuHandle,
					int(currBackBufferIndex),
					rtvDescriptorSize
		};
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Graphics::DepthStencilView()
	{
		return dsvHeap->GetHandleForStart().cpuHandle;
	}

	

}