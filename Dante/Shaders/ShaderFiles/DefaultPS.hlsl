
Texture2D albedoTex : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

struct PS_Input
{
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

float4 main(PS_Input input) : SV_TARGET
{
    return albedoTex.Sample(gsamLinearWrap, input.tc);
}