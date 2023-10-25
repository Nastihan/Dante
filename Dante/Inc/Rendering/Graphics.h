#pragma once

namespace Dante::Rendering
{
	class Graphics
	{
	public:
		Graphics() = default;
		DISABLE_COPY_AND_MOVE(Graphics)
		~Graphics() { if(device) FlushCmdQueue(); };
		void Init();
	private:
		void OnResize();
		void InitDirect3D();
		void SetupDebugLayer();
		void SelectAdapter();
		void CreateDevice();
		void CreateCommandObjects();
		void CreateSwapChain();
		void CreateRtvAndDsvDescriptorHeap();
	public:
		void Present();
		void FlushCmdQueue();
	private:
		void BuildPSOs();
		void BuildRootSigs();
		void BuildShaders();


	public:
		// Getters
		ID3D12Device* GetDevice();
		ID3D12CommandAllocator* GetCmdAllocator();
		ID3D12GraphicsCommandList* GetCmdList();
		ID3D12CommandQueue* GetCmdQueue();
		D3D12_VIEWPORT GetViewport();
		D3D12_RECT GetScissorRect();
		// Utils
		ID3D12Resource* CurrentBackBuffer();
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();


	private:
		static constexpr UINT BACK_BUFFER_COUNT = 3;
		bool msaaEnabled = false;
		UINT msaaQuality = 0;

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

		// buffers
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BACK_BUFFER_COUNT> backBuffers;
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT currBackBufferIndex{};
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// DescriptorHeaps
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		UINT rtvDescriptorSize{};
		UINT dsvDescriptorSize{};
		UINT cbvSrvUavDescriptorSize{};

		// Viewport and Scissor rect
		D3D12_VIEWPORT screenViewport;
		D3D12_RECT scissorRect;

	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSigs;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pSOs;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;


	};

}