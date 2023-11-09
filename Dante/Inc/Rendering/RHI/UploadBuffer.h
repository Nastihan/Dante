#pragma once
#include "Pch.h"
#include "Utils/DXUtil.h"
#include "Rendering/Graphics.h"
namespace Dante::Rendering::RHI
{
	template <typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(Graphics& gfx, UINT elementCount, bool isCBuffer)
			: isCBuffer(isCBuffer)
		{
			elementByteSize = sizeof(T);

			if (isCBuffer)
				elementByteSize = Utils::DXUtil::CalcCBufferByteSize(sizeof(T));

			Chk(gfx.GetDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(elementByteSize * elementCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				ID(uploadBuffer)));

			Chk(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
		}
		DISABLE_COPY_AND_MOVE(UploadBuffer)
		~UploadBuffer()
		{
			if (uploadBuffer)
			{
				uploadBuffer->Unmap(0, nullptr);
			}
			mappedData = nullptr;
		}
		
		[[nodiscard]] ID3D12Resource* Resource() const
		{
			return uploadBuffer.Get();
		}

		void CopyData(int elementIndex, const T& data)
		{
			memcpy(&mappedData[elementIndex * elementByteSize], &data, sizeof(T));
		}


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		BYTE* mappedData = nullptr;
		UINT elementByteSize = 0;
		bool isCBuffer = false;
	};
}