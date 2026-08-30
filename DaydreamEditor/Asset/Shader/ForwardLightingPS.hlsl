struct PSInput
{
    float4 position : SV_Position;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};
    
struct PSOutput
{
    float4 color : SV_Target0;
};

//================================================================================
// 광원 구조체
//================================================================================
struct DirectionalLight
{
    float3 direction;
    float intensity;
    float3 color;
    float padding1;
};

struct PointLight
{
    float3 position;
    float range;
    float3 color;
    float intensity;
};

struct SpotLight
{
    float3 position;
    float range;
    float3 direction;
    float intensity;
    float3 color;
    float innerConeCos;
    float3 padding1;
    float outerConeCos;
};

//================================================================================
// 상수 버퍼 (Constant Buffers)
//================================================================================
cbuffer Lights : register(b2)
{
    DirectionalLight dirLights[2];
    PointLight pointLights[4];
    SpotLight spotLights[4];
    float3 eyePosition;
    uint dirLightCount;
    float2 padding1;
    uint pointLightCount;
    uint spotLightCount;
};

cbuffer Material : register(b3)
{
    uint useAlbedoMap;
    uint useNormalMap;
    uint useAOMap;
    uint useMetallicMap;
    uint useRoughnessMap;
    float exposure;
    float gamma;
    float roughnessMat;
    float3 albedoMat; // baseColor
    float metallicMat;
}

[[vk::combinedImageSampler]][[vk::binding(4, 0)]]
Texture2D AlbedoTexture : register(t0);
[[vk::combinedImageSampler]][[vk::binding(4, 0)]]
SamplerState AlbedoTextureSampler : register(s0);

[[vk::combinedImageSampler]][[vk::binding(5, 0)]]
Texture2D NormalTexture : register(t1);
[[vk::combinedImageSampler]][[vk::binding(5, 0)]]
SamplerState NormalTextureSampler : register(s1);

[[vk::combinedImageSampler]][[vk::binding(6, 0)]]
Texture2D RoughnessTexture : register(t2);
[[vk::combinedImageSampler]][[vk::binding(6, 0)]]
SamplerState RoughnessTextureSampler : register(s2);

[[vk::combinedImageSampler]][[vk::binding(7, 0)]]
Texture2D MetallicTexture : register(t3);
[[vk::combinedImageSampler]][[vk::binding(7, 0)]]
SamplerState MetallicTextureSampler : register(s3);

[[vk::combinedImageSampler]][[vk::binding(8, 0)]]
Texture2D AOTexture : register(t4);
[[vk::combinedImageSampler]][[vk::binding(8, 0)]]
SamplerState AOTextureSampler : register(s4);

[[vk::combinedImageSampler]][[vk::binding(9, 0)]]
TextureCube IrradianceTexture : register(t5);
[[vk::combinedImageSampler]][[vk::binding(9, 0)]]
SamplerState IrradianceTextureSampler : register(s5);

[[vk::combinedImageSampler]][[vk::binding(10, 0)]]
TextureCube Prefilter : register(t6);
[[vk::combinedImageSampler]][[vk::binding(10, 0)]]
SamplerState PrefilterSampler : register(s6);

[[vk::combinedImageSampler]][[vk::binding(11, 0)]]
Texture2D BRDFLUT : register(t7);
[[vk::combinedImageSampler]][[vk::binding(11, 0)]]
SamplerState BRDFLUTSampler : register(s7);

//================================================================================
// PBR 함수들 (PBR 셰이더에서 가져옴)
//================================================================================
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
    return nom / max(denom, 0.0001); // 분모 0 방지
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / max(denom, 0.0001); // 분모 0 방지
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

//================================================================================
// PBR 기반 광원 계산 함수
//================================================================================
float3 CalculatePBR(float3 L, float3 V, float3 N, float3 F0, float3 albedo, float metallic, float roughness, float3 radiance)
{
    float3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.00001);
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic;
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 ComputePBRDirectionalLight(DirectionalLight light, float3 V, float3 N, float3 F0, float3 albedo, float metallic, float roughness)
{
    float3 L = normalize(-light.direction);
    float3 radiance = light.color * light.intensity;
    return CalculatePBR(L, V, N, F0, albedo, metallic, roughness, radiance);
}

float3 ComputePBRPointLight(PointLight light, float3 worldPos, float3 V, float3 N, float3 F0, float3 albedo, float metallic, float roughness)
{
    float3 L = light.position - worldPos;
    float distance = length(L);
    if (distance > light.range)
    {
        return float3(0.0, 0.0, 0.0);
    }
    L = normalize(L);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = light.color * light.intensity * attenuation;
    return CalculatePBR(L, V, N, F0, albedo, metallic, roughness, radiance);
}

float3 ComputePBRSpotLight(SpotLight light, float3 worldPos, float3 V, float3 N, float3 F0, float3 albedo, float metallic, float roughness)
{
    float3 L = light.position - worldPos;
    float distance = length(L);
    if (distance > light.range)
    {
        return float3(0.0, 0.0, 0.0);
    }
    L = normalize(L);
    float theta = dot(L, normalize(-light.direction));
    float spotEffect = smoothstep(light.outerConeCos, light.innerConeCos, theta);
    float attenuation = 1.0 / (distance * distance + 0.001);
    float3 radiance = light.color * light.intensity * attenuation * spotEffect;
    return CalculatePBR(L, V, N, F0, albedo, metallic, roughness, radiance);
}

PSOutput PSMain(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    
 // --- 1. 재질(Material) 속성 정의 (텍스처에서 읽어오도록 수정) ---
    float3 albedo = albedoMat;
    float3 normal = input.normal;
    float metallic = metallicMat;
    float roughness = roughnessMat;
    float ao = 1.0f;
    if (useAlbedoMap)
    {
        albedo = AlbedoTexture.Sample(AlbedoTextureSampler, input.uv).rgb;
    }
    if (useNormalMap)
    {
        normal = NormalTexture.Sample(NormalTextureSampler, input.uv).rgb * 2.0 - 1.0;
    }
    
    //float3 albedo = 1.0f;
    if (useMetallicMap)
    {
        metallic = MetallicTexture.Sample(MetallicTextureSampler, input.uv).b;
    }
    //float metallic= 0.0f;
    if (useRoughnessMap)
    {
        roughness = RoughnessTexture.Sample(RoughnessTextureSampler, input.uv).g;
    }
    //float roughness = 1.0f;
    if (useAOMap)
    {
        ao = AOTexture.Sample(AOTextureSampler, input.uv).r;
    }
    //float ao = 0;

    if (AlbedoTexture.Sample(AlbedoTextureSampler, input.uv).a < 0.2f)
    {
        clip(-1);
    }
    
    // --- 2. 법선(Normal) 계산 (노멀 맵핑) ---
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    N = normalize(mul(normal, TBN));

    // --- 3. 기본 벡터 및 F0 계산 ---
    float3 V = normalize(eyePosition - input.worldPosition); // View direction
    float3 R = reflect(-V, N); // Reflection vector for IBL

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    // --- 4. 직접 조명(Direct Lighting) 계산 ---
    float3 Lo = float3(0.0, 0.0, 0.0); // Outgoing Radiance
    uint i;
    for (i = 0; i < dirLightCount; ++i)
    {
        Lo += ComputePBRDirectionalLight(dirLights[i], V, N, F0, albedo, metallic, roughness);
    }
    for (i = 0; i < pointLightCount; ++i)
    {
        Lo += ComputePBRPointLight(pointLights[i], input.worldPosition, V, N, F0, albedo, metallic, roughness);
    }
    for (i = 0; i < spotLightCount; ++i)
    {
        Lo += ComputePBRSpotLight(spotLights[i], input.worldPosition, V, N, F0, albedo, metallic, roughness);
    }

    // --- 5. 간접 조명(Indirect/Ambient Lighting) - IBL 계산 ---
    float3 F_ibl = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    float3 kS_ibl = F_ibl;
    float3 kD_ibl = 1.0 - kS_ibl;
    kD_ibl *= 1.0 - metallic;
    
    float3 irradiance = IrradianceTexture.Sample(IrradianceTextureSampler, N).rgb;
    float3 diffuseIBL = (irradiance * albedo) / PI;
    
    uint width;
    uint height;
    uint maxMipLevel;
    Prefilter.GetDimensions(0, width, height, maxMipLevel);
    
    const float MAX_REFLECTION_LOD = maxMipLevel;
    float3 prefilteredColor = Prefilter.SampleLevel(PrefilterSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    //float3 prefilteredColor = Prefilter.Sample(PrefilterSampler, R).rgb;
    float2 brdf = BRDFLUT.Sample(BRDFLUTSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 specularIBL = prefilteredColor * (F_ibl * brdf.x + brdf.y);
    
    float3 ambient = (kD_ibl * diffuseIBL + specularIBL) * ao;
    
    // --- 6. 최종 색상 조합 ---
    float3 color = Lo + ambient;
    //color = diffuseIBL * albedo;

    // --- 7. 후처리: HDR 톤 매핑 및 감마 보정 ---
    color = color / (color + float3(1.0, 1.0, 1.0)); // Reinhard Tonemapping
    color = color * exposure;
    color = pow(color, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma)); // Gamma Correction

    output.color = float4(color, 1.0);
    //float3 x = normalTex - normalTex + irradiance - irradiance + prefilteredColor - prefilteredColor + float3(brdf - brdf, 0.0);

    //output.color = float4(Lo, 1.0f) + float4(albedo, 1.0) + float4(x, 1.0) + float4(metallic - metallic + roughness - roughness + ao - ao, 0.0, 0.0, 1.0) + float4(x, 1.0);
    return output;
}