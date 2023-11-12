#include "ShaderIncludes/Common.hlsli"

struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

struct VS_Output
{
    float3 posW : POS_WORLD;
    float4 posV : POS_VIEW;
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

VS_Output main( VS_Input input)
{
    VS_Output output;
    float4 posW = float4(mul(float4(input.pos, 1.0f), objectCB.world));
    output.posW = posW.xyz;
    output.posV = float4(mul(posW, passCB.view));
    output.posH = mul(posW, passCB.viewProj);
    output.normal = mul(input.normal, (float3x3) objectCB.world);
    output.tc = input.tc;
	return output;
}