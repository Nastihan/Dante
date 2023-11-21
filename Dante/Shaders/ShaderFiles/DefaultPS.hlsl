
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 1
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "ShaderIncludes/Common.hlsli"

struct PS_Input
{
    float3 posW : POS_WORLD;
    float4 posV : POS_VIEW;
    float4 posH : SV_Position;
    float3 normal : NORMAL;
    float2 tc : TEXCOORD;
    float3 tangent : TANGENT;
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

float4 main(PS_Input input) : SV_TARGET
{
    float4 diffuseAlbedo = objectCB.diffuseAlbedo;
    float3 fresnelR0 = objectCB.fresnelR0;
    float shininess = objectCB.shininess;
    uint albedoMapIndex = objectCB.albedoMapIndex;
    uint normalMapIndex = objectCB.normalMapIndex;
    // renormalize
    input.normal = normalize(input.normal);
    if (normalMapIndex != -1)
    {
        Texture2D normalTexture = ResourceDescriptorHeap[normalMapIndex];
        float4 normalSample = normalTexture.Sample(samAnisotropicWrap, input.tc);
        float3 normal = NormalSampleToWorldSpace(normalSample.rgb, input.normal, input.tangent);
        input.normal = normal;
    }
    
    Texture2D albedoTexture = ResourceDescriptorHeap[albedoMapIndex];
    // alpha testing
    if (albedoTexture.Sample(samAnisotropicWrap, input.tc).a < 0.1f)
    {
        clip(-1);
    }
    if (albedoMapIndex!= -1)
    {
        diffuseAlbedo *= albedoTexture.Sample(samAnisotropicWrap, input.tc);
    }
    
    
    float3 toEyeW = normalize(passCB.eyePosW -input.posW);
    
    float4 ambient = passCB.ambientLight * diffuseAlbedo;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(passCB.lights, mat, input.posW, input.normal, toEyeW, float3(1.0f, 1.0f, 1.0f));
    float4 litColor = directLight + ambient;
    float3 r = reflect(-toEyeW, input.normal);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, input.normal, r);
    //litColor.rgb += shininess * fresnelFactor ;
    
    litColor.a = diffuseAlbedo.a;
    
    return litColor ;
}