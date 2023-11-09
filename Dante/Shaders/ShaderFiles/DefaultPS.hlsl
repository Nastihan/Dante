
//Texture2D albedoTex : register(t0);

SamplerState samPointWrap : register(s0);
//SamplerState samPointClamp : register(s1);
//SamplerState samLinearWrap : register(s2);
//SamplerState samLinearClamp : register(s3);
//SamplerState samAnisotropicWrap : register(s4);
//SamplerState samAnisotropicClamp : register(s5);

struct ObjectCB
{
    float4x4 world;
    uint albedoMapIndex;
};
ConstantBuffer<ObjectCB> objectCB : register(b1);

struct PassCB
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
    float3 eyePosW;
};
ConstantBuffer<PassCB> passCB : register(b0);

struct PS_Input
{
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

float4 main(PS_Input input) : SV_TARGET
{
    Texture2D albedoTexture = ResourceDescriptorHeap[objectCB.albedoMapIndex];
    return albedoTexture.Sample(samPointWrap, input.tc);
}