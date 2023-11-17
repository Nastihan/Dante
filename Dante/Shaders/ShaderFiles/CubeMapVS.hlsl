#include "ShaderIncludes/Common.hlsli"
struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

struct VS_Output
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

VS_Output main( VS_Input input)
{
    VS_Output output;
    output.posL = input.pos;
    float4 posW = mul(float4(input.pos, 1.0f), objectCB.world);
    posW.xyz += passCB.eyePosW;
    output.posH = mul(posW, passCB.viewProj).xyww;
    return output;
}