#include "DaydreamPCH.h"
#include "RenderCacheManager.h"

#include "Daydream/Graphics/Resources/ShaderPipeline.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Resources/Shader.h"
#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream
{
	RenderCacheManager::RenderCacheManager()
	{
		psoCache = MakeUnique<PipelineStateCache>();
		samplerCache = MakeUnique<SamplerCache>();
	}

	void RenderCacheManager::Init()
	{
		if (instance)
		{
			return;
		}
		instance = new RenderCacheManager();
	}

	void RenderCacheManager::Shutdown()
	{
		instance->registryList.clear();

		delete instance;
	}

	//ResourceManager::ResourceManager()
	//{
	//	//meshManager = MakeUnique<MeshManager>();
	//	//modelManager = MakeUnique<ModelManager>();
	//	//shaderManager = MakeUnique<ShaderManager>();
	//	//shaderGroupManager = MakeUnique<ShaderGroupRegistry>();
	//	//samplerManager = MakeUnique<SamplerRegistry>();
	//	//textureManager = MakeUnique<Texture2DManager>();
	//	//renderPassManager = MakeUnique<RenderPassManager>();
	//	//pipelineStateManager = MakeUnique<PipelineStateManager>();
	//}

	//void ResourceManager::Init()
	//{
	//	DAYDREAM_CORE_ASSERT(!instance, "ResourceManager must be initialized only 1 time");
	//	instance = new ResourceManager();

	//	//	//instance->meshManager->CreateEssentialMeshes();
	//	//	instance->samplerManager->CreateEssentialSamplers();
	//	//	instance->textureManager->CreateEssentialTextures();
	//	//	instance->textureManager->LoadTexturesFromDirectory("Asset", true);
	//	//	instance->textureManager->LoadTexturesFromDirectory("Resource", true);
	//	//	instance->shaderManager->LoadShadersFromDirectory("Asset/Shader", true);
	//	//	//instance->shaderGroupManager->Init();
	//	//	instance->renderPassManager->CreateEssentialRenderPasses();
	//	//	instance->pipelineStateManager->CreateEssentialPipelineStates();
	//	//	instance->modelManager->LoadModelsFromDirectory("Asset", true);
	//	//}

	//	//void ResourceManager::Shutdown()
	//	//{
	//	//	instance->pipelineStateManager = nullptr;
	//	//	instance->renderPassManager = nullptr;
	//	//	instance->textureManager = nullptr;
	//	//	instance->samplerManager = nullptr;
	//	//	instance->shaderGroupManager = nullptr;
	//	//	instance->shaderManager = nullptr;
	//	//	instance->meshManager = nullptr;

	//	delete instance;
	//}
}

