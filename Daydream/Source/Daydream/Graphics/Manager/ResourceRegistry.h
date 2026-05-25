#pragma once

namespace Daydream
{
	class IResourceRegistry
	{
	public:
		virtual ~IResourceRegistry() = default;

		virtual void CreateBuiltinResources() = 0;
	};

	template<typename T>
	class ResourceRegistry : public IResourceRegistry
	{
	public:
		virtual T* Get(const String& _name)
		{
			auto itr = registry.find(_name);
			if (itr == registry.end())
			{
				return nullptr;
			}
			return itr->second.get();
		}

		virtual void Register(const String& _name, Shared<T> _resource)
		{
			auto itr = registry.find(_name);
			if (itr != registry.end())
			{
				//DAYDREAM_CORE_INFO("Resource {0} changed {1} -> {2}", _name, itr->second, _resource.get());
			}
			itr->second = std::move(_resource);
		}

	protected:
		HashMap<String, Shared<T>> registry;
	};
}
