struct PSInput
{
    float4 position : SV_Position;
    float3 worldPosition : POSITION;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

cbuffer Roughness : register(b1)
{
    float3 dummy;
    float roughness;
}

[[vk::combinedImageSampler]][[vk::binding(2)]]
TextureCube TextureCubemap : register(t0);
[[vk::combinedImageSampler]][[vk::binding(2)]]
SamplerState TextureCubemapSampler : register(s0);

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float2 Hammersley(uint i, uint N)
{
    uint bits = reversebits(i); // 내장 함수 사용
    return float2(float(i) / float(N), float(bits) * 2.3283064365386963e-10);
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway floattor
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
	// from tangent-space H floattor to world-space sample floattor
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 T = normalize(cross(up, N));
    float3 B = normalize(cross(N, T));
	
    float3 samplefloat = T* H.x + B * H.y + N * H.z;
    return normalize(samplefloat);
}

PSOutput PSMain(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    
    float3 N = normalize(input.worldPosition);
    float3 V = N;

    const uint SAMPLE_COUNT = 4096u;
    float3 prefilteredColor = float3(0.0, 0.0, 0.0);
    float totalWeight = 0.0;
    
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, N, roughness); 
        float3 L = normalize(2.0 * dot(V, H) * H - V); 

        float NdotL = saturate(dot(N, L));
        if (NdotL > 0.0)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            
            prefilteredColor += TextureCubemap.SampleLevel(TextureCubemapSampler, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / max(totalWeight, 0.001);
    output.color = float4(prefilteredColor, 1.0f);
    return output;
}