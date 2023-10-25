
struct PS_Input
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(PS_Input input) : SV_TARGET
{
    return input.color;
}