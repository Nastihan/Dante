#include "Pch.h"
#include "Rendering/RHI/Texture.h"
#include <../ThirdParty/DirectX/DxTex/include/DirectXTex.h>

namespace Dante::Rendering::RHI
{
	Texture::Texture(Graphics& gfx, std::wstring filePath)
	{
		DirectX::ScratchImage image;
		Chk( DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image));

		DirectX::ScratchImage mipChain;
		Chk(DirectX::GenerateMipMaps(*image.GetImages(), DirectX::TEX_FILTER_BOX, 0, mipChain));


		const auto& chainBase = *mipChain.GetImages();

		Chk(gfx.GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(chainBase.format, (UINT)chainBase.width, (UINT)chainBase.height, 1U, 0U),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			ID(resource)
		));

		// collect subresource data 
		std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
		subresourceData.reserve(mipChain.GetImageCount());
		for (int i = 0; i < mipChain.GetImageCount(); i++)
		{
			const auto img = mipChain.GetImage(i, 0, 0);
			D3D12_SUBRESOURCE_DATA subresData{
				.pData = img->pixels,
				.RowPitch = static_cast<LONG_PTR>(img->rowPitch),
				.SlicePitch = static_cast<LONG_PTR>(img->slicePitch),
			};

			subresourceData.push_back(subresData);
		}

		UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), 0U, (UINT)subresourceData.size());
		Chk(gfx.GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			ID(uploadResource)
		));

		UpdateSubresources(gfx.GetCmdList(), resource.Get(), uploadResource.Get(),
			0U, 0U, (UINT)subresourceData.size(), subresourceData.data());

		gfx.GetCmdList()->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		gfx.GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, gfx.CbvSrvHeap().GetCurrHandle().cpuHandle);
		gfx.CbvSrvHeap().OffsetCurrHandle();
	}
}