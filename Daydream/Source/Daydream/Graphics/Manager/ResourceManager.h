#pragma once

#include "ResourceRegistry.h"

namespace Daydream
{
	class ResourceManager
	{
	public:
		~ResourceManager() = default;

		static ResourceManager& GetInstance()
		{
			return *instance;
		}

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
		ResourceManager();

	private:
		HashMap<std::type_index, Unique<IResourceRegistry>> registryList;

		inline static ResourceManager* instance = nullptr;
	};
}
