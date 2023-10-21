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
		void CreateSwapChain();


	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory;
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

		bool msaaEnabled = false;

		// formats
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		static constexpr UINT BACK_BUFFER_COUNT = 3;


	};

}