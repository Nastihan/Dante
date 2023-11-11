#include "Common.hlsli"

struct PS_Input
{
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

float4 main(PS_Input input) : SV_TARGET
{
    Texture2D albedoTexture = ResourceDescriptorHeap[objectCB.albedoMapIndex];
    return albedoTexture.Sample(samAnisotropicWrap, input.tc);
}