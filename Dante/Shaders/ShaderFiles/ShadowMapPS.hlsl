
struct PS_Input
{
    float4 posH : SV_Position;
};

float4 main(PS_Input input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}