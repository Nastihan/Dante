#include "Pch.h"
#include "Rendering/RHI/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/tinygltf/stb_image.h"

namespace Dante::Rendering::RHI
{
	Texture::Texture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::wstring filePath, DescriptorHeap& heap)
	{
		void* texData = nullptr;
		int width{}, height{}, comp{};

		texData = stbi_load(Utils::ToNarrow(filePath).c_str(), &width, &height, &comp, STBI_rgb_alpha);

		Chk(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			ID(resource)
		));

		UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), 0U, 1U);

		Chk(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			ID(uploadResource)
		));

		D3D12_SUBRESOURCE_DATA subResourceData
		{
			.pData = texData,
			.RowPitch = (LONG_PTR)(width * 4 ),
			.SlicePitch = (LONG_PTR)(height * 4)
		};

		UpdateSubresources(cmdList, resource.Get(), uploadResource.Get(),
			0U, 0U, 1U, &subResourceData);

		cmdList->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(resource.Get(), &srvDesc, heap.GetCurrHandle().cpuHandle);
		//heap.OffsetCurrHandle();
	}

}