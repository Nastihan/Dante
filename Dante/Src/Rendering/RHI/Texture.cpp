#include "Pch.h"
#include "Rendering/RHI/Texture.h"

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif // !TINYGLTF_IMPLEMENTATION
#ifndef TINYGLTF_USE_CPP20
#define TINYGLTF_USE_CPP20
#endif // !TINYGLTF_USE_CPP20
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif // !STB_IMAGE_WRITE_IMPLEMENTATION
#ifndef STBI_MSC_SECURE_CRT
#define STBI_MSC_SECURE_CRT
#endif // !STBI_MSC_SECURE_CRT
#include "../ThirdParty/tinygltf/tiny_gltf.h"

namespace Dante::Rendering::RHI
{
	Texture::Texture(Graphics& gfx, std::wstring filePath)
	{
		void* texData = nullptr;
		int width{}, height{}, comp{};

		texData = stbi_load(Utils::ToNarrow(filePath).c_str(), &width, &height, &comp, STBI_rgb_alpha);
		

		Chk(gfx.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1U),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			ID(resource)
		));

		UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), 0U, 1U);

		Chk(gfx.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
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

		UpdateSubresources(gfx.GetCmdList(), resource.Get(), uploadResource.Get(),
			0U, 0U, 1U, &subResourceData);

		gfx.GetCmdList()->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		gfx.GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, gfx.CbvSrvHeap().GetCurrHandle().cpuHandle);
		gfx.CbvSrvHeap().OffsetCurrHandle();
	}

	Rendering::RHI::Texture::Texture(Graphics& gfx, const void* data)
	{
	}

}