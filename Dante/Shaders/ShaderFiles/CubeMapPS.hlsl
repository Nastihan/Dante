

struct PS_Input
{
    float3 pos : POSITION;
    float4 posH : SV_Position;
};

float4 main(PS_Input input) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}