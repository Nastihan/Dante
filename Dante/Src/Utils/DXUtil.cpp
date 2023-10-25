#include "Pch.h"
#include "Utils/DXUtil.h"

namespace  Dante::Utils
{
    std::pair<ComPtr<ID3D12Resource>, ComPtr<ID3D12Resource>> 
        DXUtil::CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* data, UINT64 byteSize)
    {
        ComPtr<ID3D12Resource> defaultBuffer;
        ComPtr<ID3D12Resource> uploadeBuffer;

        Chk(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, ID(defaultBuffer)
        ));

        Chk(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr, ID(uploadeBuffer)
        ));

        D3D12_SUBRESOURCE_DATA subResourceData {};
        subResourceData.pData = data;
        subResourceData.RowPitch = byteSize;
        subResourceData.SlicePitch = byteSize;

        UpdateSubresources(cmdList, defaultBuffer.Get(), uploadeBuffer.Get(),
            0, 0, 1, &subResourceData);

        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

        
        return { std::move(defaultBuffer), std::move(uploadeBuffer) };
    }


}
