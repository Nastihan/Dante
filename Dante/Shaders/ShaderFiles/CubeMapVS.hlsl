
struct VS_Input
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
};

struct VS_Output
{
    float3 pos : POSITION;
    float4 posH : SV_Position;
};

VS_Output main( VS_Input input)
{
    VS_Output output;
    output.posH = float4(input.pos, 1.0f);
    return output;
    
}