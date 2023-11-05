#include "Pch.h"
#include "Rendering/RHI/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/tinygltf/stb_image.h"

namespace Dante::Rendering::RHI
{
	Texture::Texture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, std::wstring filePath)
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
			.RowPitch = (LONG_PTR)(width * comp),
			.SlicePitch = (LONG_PTR)(height * comp)
		};

		UpdateSubresources(cmdList, resource.Get(), uploadResource.Get(),
			0U, 0U, 1U, &subResourceData);

		cmdList->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));



	}

}