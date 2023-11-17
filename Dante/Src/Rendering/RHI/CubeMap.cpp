#include "Pch.h"
#include "Rendering/RHI/CubeMap.h"
#include "Utils/DDSTextureLoader.h"

namespace Dante::Rendering::RHI
{
	CubeMap::CubeMap(Rendering::Graphics& gfx, std::wstring filePath)
	{
		Chk(DirectX::CreateDDSTextureFromFile12(gfx.GetDevice(), gfx.GetCmdList(),
			filePath.c_str(), resource, uploadResource));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = resource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = resource->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		gfx.GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, gfx.CbvSrvHeap().GetCurrHandle().cpuHandle);
		gfx.CbvSrvHeap().OffsetCurrHandle();
	}


}