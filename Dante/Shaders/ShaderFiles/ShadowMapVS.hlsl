
struct VS_Input
{
    float3 pos : POSITION;
};

struct VS_Output
{
    float4 posH : SV_Position;
};

struct ObjectCB
{
    float4x4 world;
    float4 diffuseAlbedo;
    float3 fresnelR0;
    float shininess;
    int albedoMapIndex;
    int normalMapIndex;
};
ConstantBuffer<ObjectCB> objectCB : register(b1);

struct ShadowPassCB
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
};
ConstantBuffer<ShadowPassCB> shadowPassCB : register(b0);

VS_Output main( VS_Input input )
{
    VS_Output output;
    float4 posW = mul(float4(input.pos, 1.0f), objectCB.world);
    output.posH = mul(posW, shadowPassCB.viewProj);
	return output;
}