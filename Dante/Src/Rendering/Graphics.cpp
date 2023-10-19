#include "Pch.h"
#include "Rendering\Graphics.h"

namespace Dante::Rendering
{
	void Graphics::Init()
	{
		InitDirect3D();
	}

	void Graphics::InitDirect3D()
	{
		SetupDebugLayer();
		SelectAdapter();
		Chk(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, ID(device)));

		Chk(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, ID(fence)));

		CreateCommandObjects();

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
		ComPtr<IDXGIFactory7> factory;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;
		UINT createFactoryFlags = 0;

#ifdef _DEBUG
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif 
		Chk(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)));

		size_t maxVideoMemory{};
		ComPtr<IDXGIAdapter1> adapter1;
		ComPtr<IDXGIAdapter4> adapter4;
		for (UINT i = 0; SUCCEEDED(factory->EnumAdapters1(i, adapter1.GetAddressOf())); i++)
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

	void Graphics::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Chk(device->CreateCommandQueue(&cmdQueueDesc, ID(cmdQueue)));

		Chk(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			ID(cmdListAlloc)));

		Chk(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			cmdListAlloc.Get(),
			nullptr,
			ID(cmdList)
		));

		Chk(cmdList->Close());
	}
}