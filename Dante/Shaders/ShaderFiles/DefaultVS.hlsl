
struct VS_Input
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_Output
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct PassCB
{
    matrix view;
    matrix proj;
    matrix viewProj;
    float3 eyePosW;
};
ConstantBuffer<PassCB> passCB : register(b0);

VS_Output main( VS_Input input)
{
    VS_Output output;
    output.pos = mul(float4(input.pos, 1.0f), passCB.viewProj);
    output.color = input.color;
	return output;
}