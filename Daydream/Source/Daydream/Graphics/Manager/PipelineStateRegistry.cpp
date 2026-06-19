#include "DaydreamPCH.h"
#include "PipelineStateRegistry.h"

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Resources/BuiltinResources.h"
#include "ResourceManager.h"

namespace Daydream
{
	namespace
	{
		const GraphicsPipelineState* internalDepth = nullptr;
		const GraphicsPipelineState* internalGBuffer = nullptr;
		const GraphicsPipelineState* internalDeferred = nullptr;
		const GraphicsPipelineState* internalCubemap = nullptr;
	}

	const GraphicsPipelineState* BuiltIn::PSO::Depth() { return internalDepth; }
	const GraphicsPipelineState* BuiltIn::PSO::GBuffer() { return internalGBuffer; }
	const GraphicsPipelineState* BuiltIn::PSO::Deferred() { return internalDeferred; }
	const GraphicsPipelineState* BuiltIn::PSO::EnvironmentCubemap() { return internalCubemap; }

	PipelineStateRegistry::PipelineStateRegistry()
	{
	}

	PipelineStateRegistry::~PipelineStateRegistry()
	{
		registry.clear();
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

		DepthStencilStateDesc defaultDssDesc{};

		// 2. 2D / 포스트 프로세스용 (깊이 완전 비활성화)
		DepthStencilStateDesc depthDisableDesc{};
		depthDisableDesc.depthEnable = false;
		depthDisableDesc.depthWriteEnable = false;

		// 3. 스카이박스 전용 (Z-Test On, Z-Write Off, LessEqual)
		DepthStencilStateDesc skyboxDssDesc{};
		skyboxDssDesc.depthEnable = true;
		skyboxDssDesc.depthWriteEnable = false;
		skyboxDssDesc.depthFunc = CompareFunction::LessEqual;

		GraphicsPipelineStateDesc psoDesc;

		auto ResetPSODesc = [&]() {
			psoDesc = GraphicsPipelineStateDesc();
			psoDesc.rasterizerState = defaultRastDesc;
			psoDesc.depthStencilState = defaultDssDesc;
			};

		// ==========================================
		// 일반 모델 / G-Buffer 렌더링
		// ==========================================
		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Model");
		psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
		psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
		registry["ForwardPSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("GBuffer");
		psoDesc.renderTargetFormats = {
			RenderFormat::R16G16B16A16_FLOAT, // Position
			RenderFormat::R16G16B16A16_FLOAT, // Normal
			RenderFormat::R8G8B8A8_UNORM,     // Albedo
			RenderFormat::R8G8B8A8_UNORM,     // Material
		};
		psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
		registry["GBufferPSO"] = GraphicsPipelineState::Create(psoDesc);
		internalGBuffer = registry["GBufferPSO"].get();

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Mask");
		psoDesc.renderTargetFormats = { RenderFormat::R32_UINT };
		psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
		registry["MaskPSO"] = GraphicsPipelineState::Create(psoDesc);

		// ==========================================
		// 화면 평면 출력 및 연산
		// ==========================================

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Sprite");
		psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["SpritePSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Resize");
		psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["ResizePSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("BRDF");
		psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["BRDFPSO"] = GraphicsPipelineState::Create(psoDesc);

		//ResetPSODesc();
		//psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Resize");
		//psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		//psoDesc.depthStencilState = depthDisableDesc;
		//registry["GenerateMipsPSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("DeferredLighting");
		psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["DeferredPSO"] = GraphicsPipelineState::Create(psoDesc);
		internalDeferred = registry["DeferredPSO"].get();

		// ==========================================
		// 환경 맵 전용 렌더링 (CullMode::Front 수동 적용)
		// ==========================================
		ResetPSODesc();
		psoDesc.rasterizerState = cubemapRastDesc;
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Cubemap");
		psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
		psoDesc.depthStencilState = skyboxDssDesc; // 스카이박스 전용 DSS 덮어쓰기
		psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
		registry["CubemapPSO"] = GraphicsPipelineState::Create(psoDesc);
		internalCubemap = registry["CubemapPSO"].get();


		ResetPSODesc();
		psoDesc.rasterizerState = cubemapRastDesc;
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Equirectangular");
		psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["EquirectangularPSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.rasterizerState = cubemapRastDesc;
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Irradiance");
		psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["IrradiancePSO"] = GraphicsPipelineState::Create(psoDesc);

		ResetPSODesc();
		psoDesc.rasterizerState = cubemapRastDesc;
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Prefilter");
		psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		psoDesc.depthStencilState = depthDisableDesc;
		registry["PrefilterPSO"] = GraphicsPipelineState::Create(psoDesc);

		// ==========================================
		// 그림자 전용 렌더링
		// ==========================================
		ResetPSODesc();
		psoDesc.rasterizerState = shadowRastDesc;
		psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Depth");
		psoDesc.renderTargetFormats = {};
		psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
		registry["DepthPSO"] = GraphicsPipelineState::Create(psoDesc);
		internalDepth = registry["DepthPSO"].get();


		// ==========================================
		// 에디터 전용 렌더링
		// ==========================================
		//ResetPSODesc();
		//psoDesc.rasterizerState = shadowRastDesc;
		//psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Depth");
		//psoDesc.renderTargetFormats = { RenderFormat::R32_FLOAT };
		//registry["EntityHandle"] = GraphicsPipelineState::Create(psoDesc);
	}
}

