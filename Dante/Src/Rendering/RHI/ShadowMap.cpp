#include <Pch.h>
#include <Rendering/RHI/ShadowMap.h>

namespace Dante::Rendering::RHI
{
	ShadowMap::ShadowMap(Rendering::Graphics& gfx, UINT width, UINT height)
		:width(width), height(height)
	{

		viewport = { 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
		scissorRect = { 0, 0, (int)width, (int)height };


		Chk(gfx.GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1U, 1U, 1U, 0U, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0),
			ID(shadowMap)
		));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		gfx.GetDevice()->CreateShaderResourceView(shadowMap.Get(), &srvDesc, gfx.CbvSrvHeap().GetCurrHandle().cpuHandle);
		shadowMapIndex = gfx.CbvSrvHeap().GetCurrDescriptorIndex();
		gfx.CbvSrvHeap().OffsetCurrHandle();

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;
		gfx.GetDevice()->CreateDepthStencilView(shadowMap.Get(), &dsvDesc, gfx.DsvHeap().GetCurrHandle().cpuHandle);
		cpuDsvHandle = gfx.DsvHeap().GetCurrHandle().cpuHandle;
		gfx.DsvHeap().OffsetCurrHandle();

	}

	void ShadowMap::OnResize(Rendering::Graphics& gfx, UINT width, UINT height)
	{
		shadowMap.Reset();
		viewport = { 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
		scissorRect = { 0, 0, (int)width, (int)height };


		Chk(gfx.GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1U, 1U, 1U, 0U, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0),
			ID(shadowMap)
		));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		gfx.GetDevice()->CreateShaderResourceView(shadowMap.Get(), &srvDesc, gfx.CbvSrvHeap().GetCurrHandle().cpuHandle);
		shadowMapIndex = gfx.CbvSrvHeap().GetCurrDescriptorIndex();
		gfx.CbvSrvHeap().OffsetCurrHandle();

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;
		gfx.GetDevice()->CreateDepthStencilView(shadowMap.Get(), &dsvDesc, gfx.DsvHeap().GetCurrHandle().cpuHandle);
		cpuDsvHandle = gfx.DsvHeap().GetCurrHandle().cpuHandle;
		gfx.DsvHeap().OffsetCurrHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ShadowMap::DSV() const
	{
		return cpuDsvHandle;
	}

	int ShadowMap::GetShadowMapIndex() const
	{
		return shadowMapIndex;
	}

}