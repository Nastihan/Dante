
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
void main( VS_Input input )
{
}