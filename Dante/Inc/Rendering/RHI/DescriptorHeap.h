#pragma once
#include "Pch.h"

namespace Dante::Rendering::RHI
{
	struct Descriptor
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		UINT index = 0;
	};

	class DescriptorHeap
	{
	public:
		DescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity);
		DISABLE_COPY_AND_MOVE(DescriptorHeap)
		~DescriptorHeap();
		ID3D12DescriptorHeap* GetHeap();
		Descriptor GetHandle(UINT index);
		Descriptor GetHandleForStart();
		Descriptor GetCurrHandle() const;
		UINT GetDescriptorIndex(const Descriptor& descriptor) const;
		UINT GetCurrDescriptorIndex() const;
		void OffsetCurrHandle();


	private:
		ID3D12Device* device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		UINT descriptorSize{};
		Descriptor startDescriptor;
		Descriptor currDescriptor;
		UINT capacity = 0;
	};
}
