#include "Pch.h"
#include "Rendering/RHI/DescriptorHeap.h"

namespace Dante::Rendering::RHI
{
	DescriptorHeap::DescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity)
		: device(device), capacity(capacity)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = type;
		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type ==  D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		{
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}
		else
		{
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = capacity;

		Chk(device->CreateDescriptorHeap(&heapDesc, ID(heap)));

		descriptorSize = device->GetDescriptorHandleIncrementSize(type);

		startDescriptor.cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
			startDescriptor.gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
		startDescriptor.index = 0;
		currDescriptor = startDescriptor;
	}

	DescriptorHeap::~DescriptorHeap()
	{
		device = nullptr;
	}

	ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
	{
		return heap.Get();
	}

	Descriptor DescriptorHeap::GetHandle(UINT index)
	{
		return Descriptor();
	}

	Descriptor DescriptorHeap::GetHandleForStart()
	{
		return startDescriptor;
	}

	Descriptor DescriptorHeap::GetCurrHandle() const
	{
		return currDescriptor;
	}

	UINT DescriptorHeap::GetDescriptorIndex(const Descriptor& descriptor) const
	{
		return (UINT)(descriptor.cpuHandle.ptr - startDescriptor.cpuHandle.ptr) / descriptorSize;
	}

	UINT DescriptorHeap::GetCurrDescriptorIndex() const
	{
		return GetDescriptorIndex(currDescriptor);
	}

	void DescriptorHeap::OffsetCurrHandle()
	{
		currDescriptor.cpuHandle.Offset(descriptorSize);
		currDescriptor.gpuHandle.Offset(descriptorSize);
	}



}