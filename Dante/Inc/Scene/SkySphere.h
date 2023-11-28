#pragma once
#include "Pch.h"
#include "Rendering/RHI/VertexBuffer.h"
#include "Rendering/RHI/IndexBuffer.h"
#include "Rendering/RHI/UploadBuffer.h"
#include "Utils/NastihanMath.h"
#include "Utils/GeometryGenerator.h"
#include <Rendering/RHI/Texture.h>

namespace Dante::Scene
{
	

	class SkySphere
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 tc;
			DirectX::XMFLOAT3 tangents;
		};

		struct ObjectCB
		{
			DirectX::XMFLOAT4X4 world = Utils::NastihanMath::Identity4x4();
			UINT cubeMapTexIndex = -1;
		};

	public:
		SkySphere(Rendering::Graphics& gfx);
		void Draw(Rendering::Graphics& gfx);

	private:
		std::unique_ptr<Rendering::RHI::UploadBuffer<SkySphere::ObjectCB>> objectCB;
		std::unique_ptr<Dante::Rendering::RHI::VertexBuffer<SkySphere::Vertex>> vertexBuffer;
		std::unique_ptr<Dante::Rendering::RHI::IndexBuffer> indexBuffer;
		UINT indexCount;
		std::unique_ptr<Dante::Rendering::RHI::Texture> cubeMapTex;
	};

}