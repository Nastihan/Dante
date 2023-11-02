
struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
};

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
    output.pos = mul(float4(input.pos, 1.0f), passCB.viewProj);
    output.normal = input.normal;
	return output;
}