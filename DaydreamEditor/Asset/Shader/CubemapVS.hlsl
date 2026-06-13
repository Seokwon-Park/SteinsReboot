struct VSInput
{
    float3 position : POSITION;
};

cbuffer Camera : register(b0)
{
    matrix view;
    matrix projection;
    matrix viewProjection;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 worldPosition : POSITION;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    output.position = mul(float4(input.position, 1.0f), viewProjection).xyww;
    output.worldPosition = input.position;

    return output;
}
