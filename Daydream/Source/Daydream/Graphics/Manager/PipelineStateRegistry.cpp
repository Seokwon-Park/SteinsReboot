#include "DaydreamPCH.h"
#include "PipelineStateRegistry.h"

#include "Daydream/Graphics/Resources/PipelineState/GraphicsPipelineState.h"
#include "ResourceManager.h"

namespace Daydream
{
	PipelineStateRegistry::PipelineStateRegistry()
	{
	}

    PipelineStateRegistry::~PipelineStateRegistry()
    {
        DepthPSO = nullptr;
        GBufferPSO = nullptr;
        DeferredPSO = nullptr;
    }

	void PipelineStateRegistry::CreateBuiltinResources()
	{
		RasterizerStateDesc defaultRastDesc = {};

		RasterizerStateDesc cubemapRastDesc = {};
		cubemapRastDesc.cullMode = CullMode::Front;

		RasterizerStateDesc shadowRastDesc = {};
		shadowRastDesc.cullMode = CullMode::Front;
		shadowRastDesc.fillMode = FillMode::Solid;
		shadowRastDesc.depthBias = 1000;
		shadowRastDesc.slopeScaledDepthBias = 1.0f;
		shadowRastDesc.depthClipEnable = true;

        GraphicsPipelineStateDesc psoDesc;

        auto ResetPSO = [&]() {
            psoDesc = GraphicsPipelineStateDesc();
            psoDesc.rasterizerState = defaultRastDesc;
            };

        // ==========================================
        // ÀÏ¹Ý ¸ðµ¨ / G-Buffer ·»´õ¸µ
        // ==========================================
        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Model");
        psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
        psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
        registry["ForwardPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Sprite");
        psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
        registry["SpritePSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Resize");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        registry["ResizePSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("BRDF");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        registry["BRDFPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Mip");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        psoDesc.depthStencilFormat = RenderFormat::UNKNOWN;
        registry["GenerateMipsPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("GBuffer");
        psoDesc.renderTargetFormats = {
            RenderFormat::R16G16B16A16_FLOAT, // Position
            RenderFormat::R16G16B16A16_FLOAT, // Normal
            RenderFormat::R8G8B8A8_UNORM,     // Albedo
            RenderFormat::R8G8B8A8_UNORM,     // Material
            RenderFormat::R32_UINT            // EntityID
        };
        psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
        registry["GBufferPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("DeferredLighting");
        psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
        psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
        registry["DeferredPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Mask");
        psoDesc.renderTargetFormats = { RenderFormat::R32_UINT };
        psoDesc.depthStencilFormat = RenderFormat::UNKNOWN;
        registry["MaskPSO"] = GraphicsPipelineState::Create(psoDesc);

        // ==========================================
        // È¯°æ ¸Ê Àü¿ë ·»´õ¸µ (CullMode::Front ¼öµ¿ Àû¿ë)
        // ==========================================
        ResetPSO();
        psoDesc.rasterizerState = cubemapRastDesc; 
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Cubemap");
        psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
        psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
        registry["CubemapPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.rasterizerState = cubemapRastDesc; 
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Equirectangular");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        registry["EquirectangularPSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.rasterizerState = cubemapRastDesc; 
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Irradiance");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        registry["IrradiancePSO"] = GraphicsPipelineState::Create(psoDesc);

        ResetPSO();
        psoDesc.rasterizerState = cubemapRastDesc; 
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Prefilter");
        psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
        registry["PrefilterPSO"] = GraphicsPipelineState::Create(psoDesc);

        // ==========================================
        // ±×¸²ÀÚ Àü¿ë ·»´õ¸µ
        // ==========================================
        ResetPSO();
        psoDesc.rasterizerState = shadowRastDesc; // ¼¨µµ¿ì ±×¸± ¶§¸¸ µ¤¾î¾º¿ò
        psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Depth");
        psoDesc.renderTargetFormats = {};
        psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
        registry["DepthPSO"] = GraphicsPipelineState::Create(psoDesc);
	}
}

