
struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

struct VS_Output
{
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

struct ObjectCB
{
    float4x4 world;
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

VS_Output main( VS_Input input)
{
    VS_Output output;
    float4 posW = float4(mul(float4(input.pos, 1.0f), objectCB.world));
    output.posH = mul(posW, passCB.viewProj);
    output.normal = input.normal;
    output.tc = input.tc;
	return output;
}