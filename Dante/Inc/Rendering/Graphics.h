#pragma once

namespace Dante::Rendering
{
	class Graphics
	{
	public:
		Graphics() = default;
		DISABLE_COPY_AND_MOVE(Graphics)
		//~Graphics() { if(device) FlushCmdQueue(); };
		void Init();

	private:
		void OnResize();

		void InitDirect3D();
		void SetupDebugLayer();
		void SelectAdapter();
		void CreateDevice();
		void CreateCommandObjects();
		void CreateSwapChain();
		
		void CreateRtvDescriptorHeap();

		void FlushCmdQueue();


	private:
		static constexpr UINT BACK_BUFFER_COUNT = 3;
		bool msaaEnabled = false;
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Graphics device
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory;
		Microsoft::WRL::ComPtr<ID3D12Device10> device{};
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter{};
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain{};

		// Command objects
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue{};
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mainCmdListAlloc{};
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList{};
		Microsoft::WRL::ComPtr<ID3D12Fence1> fence{};
		UINT64 fenceVal{ 0 };

		// Back buffers
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BACK_BUFFER_COUNT> backBuffers;
		UINT currBackBufferIndex{};

		// DescriptorHeaps
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		UINT rtvDescriptorSize{};
		UINT dsvDescriptorSize{};
		UINT cbvSrvUavDescriptorSize{};

		// Viewport and Scissor rect
		D3D12_VIEWPORT screenViewport;
		D3D12_RECT scissorRect;


	};

}