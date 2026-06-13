struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};


struct VSOutput
{
    float4 position : SV_Position;
};

cbuffer World : register(b0)
{
    matrix world;
    matrix invTranspose;
};

cbuffer LightViewProjection : register(b1)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(float4(input.position, 1.0f), world);
    float3 dummy = input.normal - input.normal + input.tangent - input.tangent + float3(input.uv - input.uv, 0.0f);
    float4 dummyy = float4(dummy, 0.0f);
    output.position = mul(worldPos, viewProjection) + dummyy - dummyy;

    return output;
}
