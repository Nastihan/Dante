#pragma once

namespace Dante::Rendering
{
	class Graphics
	{
	public:
		Graphics() = default;
		DISABLE_COPY_AND_MOVE(Graphics)
			~Graphics() = default;
		void Init();

	private:
		void InitDirect3D();
		void SetupDebugLayer();
		void SelectAdapter();
		void CreateCommandObjects();


	private:
		Microsoft::WRL::ComPtr<ID3D12Device10> device{};
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter{};
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain{};

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue{};
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdListAlloc{};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList{};

		Microsoft::WRL::ComPtr<ID3D12Fence1> fence{};

		UINT RtvDescriptorSize{};
		UINT DsvDescriptorSize{};
		UINT CbvSrvUavDescriptorSize{};


	};

}