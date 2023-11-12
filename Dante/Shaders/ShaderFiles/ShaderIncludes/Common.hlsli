#include "LightingUtil.hlsli"


struct ObjectCB
{
    float4x4 world;
    float4 diffuseAlbedo;
    float3 fresnelR0;
    float shininess;
    uint albedoMapIndex;
};
ConstantBuffer<ObjectCB> objectCB : register(b1);

struct PassCB
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
    float3 eyePosW;
    float padding;
    float4 ambientLight;
    Light lights[16];
};
ConstantBuffer<PassCB> passCB : register(b0);

// Samplers
SamplerState samPointWrap : register(s0);
SamplerState samPointClamp : register(s1);
SamplerState samLinearWrap : register(s2);
SamplerState samLinearClamp : register(s3);
SamplerState samAnisotropicWrap : register(s4);
SamplerState samAnisotropicClamp : register(s5);