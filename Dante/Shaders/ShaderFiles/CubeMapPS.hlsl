#include "ShaderIncludes/Common.hlsli"

struct PS_Input
{
    float3 posL : POSITION;
    float4 posH : SV_Position;
};

struct ObjectCB
{
    float4x4 world;
    uint skyMapTexIndex;
};
ConstantBuffer<ObjectCB> objectCB : register(b1);

float4 main(PS_Input input) : SV_Target
{
    TextureCube cubeMap = ResourceDescriptorHeap[objectCB.skyMapTexIndex];
    return cubeMap.Sample(samAnisotropicWrap, input.posL);
}