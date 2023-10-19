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


	private:
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter{};

		Microsoft::WRL::ComPtr<ID3D12Device10> device{};

		Microsoft::WRL::ComPtr<ID3D12Fence1> fence{};
	};

}