#pragma once

#include "ResourceRegistry.h"
#include "PipelineCache.h"
#include "SamplerCache.h"

namespace Daydream
{
	class RenderCacheManager
	{
	public:
		~RenderCacheManager() = default;

		static RenderCacheManager& GetInstance()
		{
			return *instance;
		}

		inline static GraphicsPipelineState* RequestPipelineState(const GraphicsPipelineStateDesc& _desc) { return instance->psoCache->RequestPipelineState(_desc); };

		template<typename ResourceType>
		static ResourceType* GetResource(const String& _name)
		{
			auto itr = instance->registryList.find(typeid(ResourceType));
			if (itr == instance->registryList.end())
			{
				DAYDREAM_CORE_WARN("Wrong Type");
				return nullptr;
			}
			ResourceRegistry<ResourceType>* registry = static_cast<ResourceRegistry<ResourceType>*>(itr->second.get());
			return registry->Get(_name);
		}

		template<typename ResourceType>
		static void Register(const String& _name, Shared<ResourceType> _resource)
		{
			auto itr = instance->registryList.find(typeid(ResourceType));
			if (itr == instance->registryList.end())
			{
				DAYDREAM_CORE_WARN("Wrong Type");
				return nullptr;
			}
			ResourceRegistry<ResourceType>* registry = static_cast<ResourceRegistry<ResourceType>*>(itr->second.get());
			registry->Register(_name, _resource);
		}

		static void Init();
		static void Shutdown();
	protected:
		RenderCacheManager();

	private:
		HashMap<std::type_index, Unique<IResourceRegistry>> registryList;

		Unique<PipelineStateCache> psoCache;
		Unique<SamplerCache> samplerCache;

		inline static RenderCacheManager* instance = nullptr;
	};
}
