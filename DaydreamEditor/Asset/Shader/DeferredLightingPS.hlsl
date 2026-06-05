struct PSInput
{
    float4 position : SV_Position;
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

cbuffer EditorData : register(b3) 
{
    uint2 padding;
    uint selectedID; // 현재 선택된 Entity ID (0 = 아무것도 선택 안됨)
    int outlineThickness;
};

cbuffer LightViewProjection : register(b4) 
{
    matrix lightView;
    matrix lightProjection;
    matrix lightViewProjection;
};


// (cbuffer Material : register(b3) ... )


//Texture2D t_NormalBuffer : register(t9);
//Texture2D t_AlbedoBuffer : register(t10);
//Texture2D t_MaterialBuffer : register(t11);

//// G-Buffer 샘플링을 위한 샘플러 (Point Sampler 권장)
//SamplerState g_PointSampler : register(s8);

[[vk::combinedImageSampler]][[vk::binding(5, 0)]]
Texture2D PositionTexture : register(t0);
[[vk::combinedImageSampler]][[vk::binding(5, 0)]]
SamplerState PositionTextureSampler : register(s0);

[[vk::combinedImageSampler]][[vk::binding(6, 0)]]
Texture2D AlbedoTexture : register(t1);
[[vk::combinedImageSampler]][[vk::binding(6, 0)]]
SamplerState AlbedoTextureSampler : register(s1);

[[vk::combinedImageSampler]][[vk::binding(7, 0)]]
Texture2D NormalTexture : register(t2);
[[vk::combinedImageSampler]][[vk::binding(7, 0)]]
SamplerState NormalTextureSampler : register(s2);

[[vk::combinedImageSampler]][[vk::binding(8, 0)]]
Texture2D RMAOTexture : register(t3);
[[vk::combinedImageSampler]][[vk::binding(8, 0)]]
SamplerState RMAOTextureSampler : register(s3);

[[vk::combinedImageSampler]][[vk::binding(9, 0)]]
TextureCube IrradianceTexture : register(t4);
[[vk::combinedImageSampler]][[vk::binding(9, 0)]]
SamplerState IrradianceTextureSampler : register(s4);

[[vk::combinedImageSampler]][[vk::binding(10, 0)]]
TextureCube Prefilter : register(t5);
[[vk::combinedImageSampler]][[vk::binding(10, 0)]]
SamplerState PrefilterSampler : register(s5);

[[vk::combinedImageSampler]][[vk::binding(11, 0)]]
Texture2D BRDFLUT : register(t6);
[[vk::combinedImageSampler]][[vk::binding(11, 0)]]
SamplerState BRDFLUTSampler : register(s6);

//[[vk::combinedImageSampler]][[vk::binding(12, 0)]]
//Texture2D<uint> EntityIDTexture : register(t7);
//[[vk::combinedImageSampler]][[vk::binding(12, 0)]]
//SamplerState EntityIDTextureSampler : register(s7);

//[[vk::combinedImageSampler]][[vk::binding(13, 0)]]
//Texture2D<uint> OutlineTexture : register(t8);
//[[vk::combinedImageSampler]][[vk::binding(13, 0)]]
//SamplerState OutlineTextureSampler : register(s8);

[[vk::combinedImageSampler]][[vk::binding(14, 0)]]
Texture2D DepthTexture : register(t9);
[[vk::combinedImageSampler]][[vk::binding(14, 0)]]
SamplerState DepthTextureSampler : register(s9);

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


// --- 라이팅 패스 픽셀 셰이더 ---
PSOutput PSMain(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    
    // --- 1. G-Buffer에서 데이터 샘플링 ---
    float2 uv = input.uv;

    //int3 pixelCoord = int3(input.position.xy, 0);
    
    //// R32 텍스처에서 값 읽기
    //uint value = OutlineTexture.Load(pixelCoord).r;
    
    //// selected id가 아닌 픽셀에 대해서 검사
    //if (value != 1 && selectedID != 0)
    //{
    //    for (int y = -outlineThickness; y <= outlineThickness; ++y)
    //    {
    //        for (int x = -outlineThickness; x <= outlineThickness; ++x)
    //        {
    //            // 자기 자신은 검사 패스
    //            if (x == 0 && y == 0)
    //                continue;

    //            // 주변 픽셀 좌표 계산
    //            int3 neighborPos = pixelCoord + int3(x, y, 0);
            
    //            uint neighborValue = OutlineTexture.Load(neighborPos).r;

    //            //내가 이웃 픽셀의 외곽선 픽셀이 되어야 하는 경우
    //            if (neighborValue == 1)
    //            {
    //                // 이웃 픽셀의 실제 값
    //                uint realVisibleID = EntityIDTexture.Load(neighborPos).r;
                
    //                if (realVisibleID == selectedID)
    //                {
    //                    // 이 픽셀이 실제로도 그 물체 근처라면 (가려지지 않음)
    //                    output.color = float4(1.0f, 0.5f, 0.0f, 1.0f); // 밝은 주황
    //                }
    //                else
    //                {
    //                    // 마스크상으로는 외곽선인데, 실제로는 다른 게 보임 (가려진 경우)
    //                    output.color = float4(0.8f, 0.3f, 0.0f, 1.0f); // 어두운 주황
    //                }
                
    //                return output;
    //            }
    //        }
    //    }
    //}

    float4 posData = PositionTexture.Sample(PositionTextureSampler, uv);
    float3 worldPosition = posData.xyz;
    
    // G-Buffer가 비어있는 픽셀(배경)은 스킵
    // (G-Buffer 셰이더에서 position.w = 1.0으로 썼으므로, 0.0이면 비어있는 것)
    if (posData.w == 0.0)
    {
        discard; // 또는 배경색(스카이박스) 출력
    }

    float3 N = NormalTexture.Sample(NormalTextureSampler, uv).xyz;
    float3 albedo = AlbedoTexture.Sample(AlbedoTextureSampler, uv).rgb;
    float4 matData = RMAOTexture.Sample(RMAOTextureSampler, uv);
    float metallic = matData.b;
    float roughness = matData.g;
    float ao = matData.r;

    // --- (G-Buffer 셰이더의 1, 2번 단계가 위 코드로 대체됨) ---
    
    // --- 3. 기본 벡터 및 F0 계산 (Forward 셰이더와 동일) ---
    // (eyePosition은 cbuffer Lights(b2)에서 가져옴)
    float3 V = normalize(eyePosition - worldPosition);
    float3 R = reflect(-V, N);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    // --- 4. 직접 조명(Direct Lighting) 계산 (Forward 셰이더와 *완전히 동일*) ---
    float3 Lo = float3(0.0, 0.0, 0.0);
    uint i;
    for (i = 0; i < dirLightCount; ++i)
    {
        float3 L = normalize(-dirLights[i].direction);

        float shadowFactor = 1.0f;
        if (i == 0)
        {
            DirectionalLight dirLight = dirLights[i];
            float4 lightScreen = mul(float4(worldPosition, 1.0f), lightViewProjection);
            lightScreen.xyz /= lightScreen.w;
        
        // 2. 카메라(광원)에서 볼 때의 텍스춰 좌표 계산
        // [-1, 1]x[-1, 1] -> [0, 1]x[0, 1]
        // 주의: 텍스춰 좌표와 NDC는 y가 반대
            float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
            lightTexcoord += 1.0f;
            lightTexcoord *= 0.5;
        
            uint width, height;
            DepthTexture.GetDimensions(width, height);
            float2 texelSize = 1.0 / float2(width, height);

            float currentDepth = lightScreen.z;
        
        // Bias 설정 (상수 0.001 대신 각도에 따른 Bias 추천하지만, 일단 기존 값 유지)
            float bias = 0.00001;
        
            float shadowSum = 0.0;

        // 4. PCF 루프 (3x3 샘플링)
            for (int x = -1; x <= 1; ++x)
            {
                for (int y = -1; y <= 1; ++y)
                {
                // 주변 텍셀 좌표 계산
                    float2 pcfUV = lightTexcoord + float2(x, y) * texelSize;
                
                // 뎁스 샘플링
                    float pcfDepth = DepthTexture.Sample(DepthTextureSampler, pcfUV).r;
                
                // 비교 로직:
                // 내 깊이(current - bias)가 맵에 기록된 깊이(pcfDepth)보다 크면(뒤에 있으면) 그림자
                // 그림자면 0.0, 빛이면 1.0을 더함 (shadowFactor가 1.0이어야 밝으니까)
                    if (currentDepth - bias > pcfDepth)
                    {
                        shadowSum += 0.0f; // 가려짐 (그림자)
                    }
                    else
                    {
                        shadowSum += 1.0f; // 안 가려짐 (빛)
                    }
                }
            }

        shadowFactor = shadowSum / 9.0;

        // --- [수정된 부분 끝] ---

        // (선택 사항) 범위 밖 처리: 섀도우 맵 밖은 항상 빛을 받도록
            if (lightScreen.z > 1.0 || lightScreen.z < 0.0 ||
            lightTexcoord.x > 1.0 || lightTexcoord.x < 0.0 ||
            lightTexcoord.y > 1.0 || lightTexcoord.y < 0.0)
            {
                shadowFactor = 1.0f;
            }
        }
        
        Lo += ComputePBRDirectionalLight(dirLights[i], V, N, F0, albedo, metallic, roughness) * shadowFactor;
    }
    for (i = 0; i < pointLightCount; ++i)
    {
        Lo += ComputePBRPointLight(pointLights[i], worldPosition, V, N, F0, albedo, metallic, roughness);
    }
    for (i = 0; i < spotLightCount; ++i)
    {
        Lo += ComputePBRSpotLight(spotLights[i], worldPosition, V, N, F0, albedo, metallic, roughness);
    }

    // --- 5. 간접 조명(IBL) 계산 ---
    float3 F_ibl = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    float3 kS_ibl = F_ibl;
    float3 kD_ibl = 1.0 - kS_ibl;
    kD_ibl *= 1.0 - metallic;
    
    float3 irradiance = IrradianceTexture.Sample(IrradianceTextureSampler, N).rgb;
    float3 diffuseIBL = (irradiance * albedo) / PI;
    
    uint width, height, maxMipLevel;
    Prefilter.GetDimensions(0, width, height, maxMipLevel);
    
    const float MAX_REFLECTION_LOD = maxMipLevel;
    float3 prefilteredColor = Prefilter.SampleLevel(PrefilterSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    float2 brdf = BRDFLUT.Sample(BRDFLUTSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 specularIBL = prefilteredColor * (F_ibl * brdf.x + brdf.y);
    
    float3 ambient = (kD_ibl * diffuseIBL + specularIBL) * ao;
    
    // --- 6. 최종 색상 조합 (Forward 셰이더와 동일) ---
    float3 color = Lo + ambient;

    // --- 7. 후처리: 톤 매핑 및 감마 보정 (Forward 셰이더와 동일) ---
    // (exposure, gamma는 cbuffer Material(b3)에서 가져옴)
    float gamma = 2.2f;
    color = color / (color + float3(1.0, 1.0, 1.0)); // Reinhard Tonemapping
    color = color * 1.0f;
    color = pow(color, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma)); // Gamma Correction
    output.color = float4(color, 1.0f);
    return output;
}