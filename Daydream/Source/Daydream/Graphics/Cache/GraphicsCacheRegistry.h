#pragma once
#include "PipelineStateCache.h"
#include "SamplerCache.h"



namespace Daydream
{
	class GraphicsCacheRegistry
	{
	public:
		~GraphicsCacheRegistry() = default;

		static GraphicsCacheRegistry& GetInstance()
		{
			return *instance;
		}

		inline static GraphicsPipelineState* RequestPipelineState(const GraphicsPipelineStateDesc& _desc) { return instance->psoCache->Request(_desc); };
		inline static Sampler* RequestSampler(const SamplerDesc& _desc) { return instance->samplerCache->Request(_desc); };

		//template<typename ResourceType>
		//static ResourceType* GetResource(const String& _name)
		//{
		//	auto itr = instance->registryList.find(typeid(ResourceType).name);
		//	if (itr == instance->registryList.end())
		//	{
		//		DAYDREAM_CORE_WARN("Wrong Type");
		//		return nullptr;
		//	}
		//	ResourceRegistry<ResourceType>* registry = static_cast<ResourceRegistry<ResourceType>*>(itr->second.get());
		//	return registry->Get(_name);
		//}

		static void Init();
		static void Shutdown();
	protected:
		GraphicsCacheRegistry();

	private:
		Unique<PipelineStateCache> psoCache;
		Unique<SamplerCache> samplerCache;

		inline static GraphicsCacheRegistry* instance = nullptr;
	};
}
