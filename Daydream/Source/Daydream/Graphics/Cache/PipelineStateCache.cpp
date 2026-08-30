#include "DaydreamPCH.h"
#include "PipelineStateCache.h"

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Resources/BuiltinResources.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

namespace Daydream
{
	namespace
	{
		const GraphicsPipelineState* internalDepth = nullptr;
		const GraphicsPipelineState* internalGBuffer = nullptr;
		const GraphicsPipelineState* internalDeferred = nullptr;
		const GraphicsPipelineState* internalCubemap = nullptr;

		void PrintHash(const GraphicsPipelineStateDesc& _desc)
		{
			// 각 부품별로 독립적인 단발성 해시 계산 (XXH64 함수 사용)
			UInt64 hash_shader = XXH64(&_desc.shaderPipeline, sizeof(Daydream::ShaderPipeline*), 0);
			UInt64 hash_rs = XXH64(&_desc.rasterizerState, sizeof(Daydream::RasterizerStateDesc), 0);
			UInt64 hash_dss = XXH64(&_desc.depthStencilState, sizeof(Daydream::DepthStencilStateDesc), 0);
			UInt64 hash_bs = XXH64(&_desc.blendState, sizeof(Daydream::BlendStateDesc), 0);
			UInt64 hash_dsf = XXH64(&_desc.depthStencilFormat, sizeof(Daydream::RenderFormat), 0);
			UInt64 hash_sample = XXH64(&_desc.sampleCount, sizeof(UInt32), 0);

			DAYDREAM_CORE_INFO("=== Data ===");
			DAYDREAM_CORE_INFO("1. Shader Address: {0}", (void*)_desc.shaderPipeline);
			DAYDREAM_CORE_INFO("2. DepthStencil Format: {0}", static_cast<int>(_desc.depthStencilFormat));
			DAYDREAM_CORE_INFO("3. Sample Count: {0}", _desc.sampleCount);
			DAYDREAM_CORE_INFO("=================================");

			DAYDREAM_CORE_INFO("=== Hash Result ===");
			DAYDREAM_CORE_INFO("1. Shader Ptr: {0}", hash_shader);
			DAYDREAM_CORE_INFO("2. Rasterizer State: {0}", hash_rs);
			DAYDREAM_CORE_INFO("3. DepthStencil State: {0}", hash_dss);
			DAYDREAM_CORE_INFO("4. Blend State: {0}", hash_bs);
			DAYDREAM_CORE_INFO("5. DepthStencil Format: {0}", hash_dsf);
			DAYDREAM_CORE_INFO("6. Sample Count: {0}", hash_sample);
			DAYDREAM_CORE_INFO("=========================================");
		}
	}

	const GraphicsPipelineState* BuiltIn::PSO::Depth() { return internalDepth; }
	const GraphicsPipelineState* BuiltIn::PSO::GBuffer() { return internalGBuffer; }
	const GraphicsPipelineState* BuiltIn::PSO::Deferred() { return internalDeferred; }
	const GraphicsPipelineState* BuiltIn::PSO::EnvironmentCubemap() { return internalCubemap; }

	PipelineStateCache::PipelineStateCache()
	{
	}

	PipelineStateCache::~PipelineStateCache()
	{
		
	}

	Shared<GraphicsPipelineState> PipelineStateCache::CreateResource(const GraphicsPipelineStateDesc& _key)
	{
		return GraphicsPipelineState::Create(_key);
	}


	UInt64 GraphicsPipelineStateHash::operator()(const GraphicsPipelineStateDesc& _desc) const
	{
		// 1. xxHash 상태 객체 생성 및 초기화 (시드값은 기본 0)
		XXH64_state_t* state = XXH64_createState();
		XXH64_reset(state, 0);
		
		XXH64_update(state, &_desc.shaderPipeline, sizeof(Daydream::ShaderPipeline*));
		XXH64_update(state, &_desc.rasterizerState, sizeof(Daydream::RasterizerStateDesc));
		XXH64_update(state, &_desc.depthStencilState, sizeof(Daydream::DepthStencilStateDesc));
		XXH64_update(state, &_desc.blendState, sizeof(Daydream::BlendStateDesc));
		XXH64_update(state, &_desc.depthStencilFormat, sizeof(Daydream::RenderFormat));
		XXH64_update(state, &_desc.sampleCount, sizeof(UInt32));
		
		if (!_desc.renderTargetFormats.empty())
		{
			XXH64_update(state,
				_desc.renderTargetFormats.data(),
				_desc.renderTargetFormats.size() * sizeof(Daydream::RenderFormat));
		}
		// 4. 최종 해시값 추출 및 메모리 반환
		XXH64_hash_t hashValue = XXH64_digest(state);
		XXH64_freeState(state);
		return Cast<UInt64>(hashValue);
	}

	//void PipelineStateRegistry::CreateBuiltinResources()
	//{
	//	RasterizerStateDesc defaultRastDesc = {};

	//	RasterizerStateDesc cubemapRastDesc = {};
	//	cubemapRastDesc.cullMode = CullMode::Front;

	//	RasterizerStateDesc shadowRastDesc = {};
	//	shadowRastDesc.cullMode = CullMode::Front;
	//	shadowRastDesc.fillMode = FillMode::Solid;
	//	shadowRastDesc.depthBias = 1000;
	//	shadowRastDesc.slopeScaledDepthBias = 1.0f;
	//	shadowRastDesc.depthClipEnable = true;

	//	DepthStencilStateDesc defaultDssDesc{};

	//	// 2. 2D / 포스트 프로세스용 (깊이 완전 비활성화)
	//	DepthStencilStateDesc depthDisableDesc{};
	//	depthDisableDesc.depthEnable = false;
	//	depthDisableDesc.depthWriteEnable = false;

	//	// 3. 스카이박스 전용 (Z-Test On, Z-Write Off, LessEqual)
	//	DepthStencilStateDesc skyboxDssDesc{};
	//	skyboxDssDesc.depthEnable = true;
	//	skyboxDssDesc.depthWriteEnable = false;
	//	skyboxDssDesc.depthFunc = CompareFunction::LessEqual;

	//	GraphicsPipelineStateDesc psoDesc;

	//	auto ResetPSODesc = [&]() {
	//		psoDesc = GraphicsPipelineStateDesc();
	//		psoDesc.rasterizerState = defaultRastDesc;
	//		psoDesc.depthStencilState = defaultDssDesc;
	//		};

	//	// ==========================================
	//	// 일반 모델 / G-Buffer 렌더링
	//	// ==========================================
	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Model");
	//	psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
	//	psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
	//	registry["ForwardPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("GBuffer");
	//	psoDesc.renderTargetFormats = {
	//		RenderFormat::R16G16B16A16_FLOAT, // Position
	//		RenderFormat::R16G16B16A16_FLOAT, // Normal
	//		RenderFormat::R8G8B8A8_UNORM,     // Albedo
	//		RenderFormat::R8G8B8A8_UNORM,     // Material
	//	};
	//	psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
	//	registry["GBufferPSO"] = GraphicsPipelineState::Create(psoDesc);
	//	internalGBuffer = registry["GBufferPSO"].get();

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Mask");
	//	psoDesc.renderTargetFormats = { RenderFormat::R32_UINT };
	//	psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
	//	registry["MaskPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	// ==========================================
	//	// 화면 평면 출력 및 연산
	//	// ==========================================

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Sprite");
	//	psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["SpritePSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Resize");
	//	psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["ResizePSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("BRDF");
	//	psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["BRDFPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	//ResetPSODesc();
	//	//psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Resize");
	//	//psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	//psoDesc.depthStencilState = depthDisableDesc;
	//	//registry["GenerateMipsPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("DeferredLighting");
	//	psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["DeferredPSO"] = GraphicsPipelineState::Create(psoDesc);
	//	internalDeferred = registry["DeferredPSO"].get();

	//	// ==========================================
	//	// 환경 맵 전용 렌더링 (CullMode::Front 수동 적용)
	//	// ==========================================
	//	ResetPSODesc();
	//	psoDesc.rasterizerState = cubemapRastDesc;
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Cubemap");
	//	psoDesc.renderTargetFormats = { RenderFormat::R8G8B8A8_UNORM };
	//	psoDesc.depthStencilState = skyboxDssDesc; // 스카이박스 전용 DSS 덮어쓰기
	//	psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
	//	registry["CubemapPSO"] = GraphicsPipelineState::Create(psoDesc);
	//	internalCubemap = registry["CubemapPSO"].get();


	//	ResetPSODesc();
	//	psoDesc.rasterizerState = cubemapRastDesc;
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Equirectangular");
	//	psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["EquirectangularPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.rasterizerState = cubemapRastDesc;
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Irradiance");
	//	psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["IrradiancePSO"] = GraphicsPipelineState::Create(psoDesc);

	//	ResetPSODesc();
	//	psoDesc.rasterizerState = cubemapRastDesc;
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Prefilter");
	//	psoDesc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
	//	psoDesc.depthStencilState = depthDisableDesc;
	//	registry["PrefilterPSO"] = GraphicsPipelineState::Create(psoDesc);

	//	// ==========================================
	//	// 그림자 전용 렌더링
	//	// ==========================================
	//	ResetPSODesc();
	//	psoDesc.rasterizerState = shadowRastDesc;
	//	psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Depth");
	//	psoDesc.renderTargetFormats = {};
	//	psoDesc.depthStencilFormat = RenderFormat::R24G8_TYPELESS;
	//	registry["DepthPSO"] = GraphicsPipelineState::Create(psoDesc);
	//	internalDepth = registry["DepthPSO"].get();


	//	// ==========================================
	//	// 에디터 전용 렌더링
	//	// ==========================================
	//	//ResetPSODesc();
	//	//psoDesc.rasterizerState = shadowRastDesc;
	//	//psoDesc.shaderGroup = ResourceManager::GetResource<ShaderGroup>("Depth");
	//	//psoDesc.renderTargetFormats = { RenderFormat::R32_FLOAT };
	//	//registry["EntityHandle"] = GraphicsPipelineState::Create(psoDesc);
	//}



}

