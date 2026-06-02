#pragma once

#include "Daydream/Scene/GameEntity/GameEntity.h"
#include "Component.h"

namespace Daydream
{
#define REGISTER_COMPONENT(ComponentType)   \
    do {                                    \
        RegisterComponent<ComponentType>(#ComponentType); \
    } while(0)
    
    using ComponentCreateFunc = std::function<Component*(GameEntity*)>;
    using ComponentHasFunc = std::function<bool(GameEntity*)>;

    struct ComponentRegistryFunctions
    {
        ComponentCreateFunc createFunc;
        ComponentHasFunc hasFunc;
    };

    // 컴포넌트 이름으로 컴포넌트를 생성

    class ComponentRegistry
    {
    public:
        // 싱글톤 인스턴스를 가져오는 함수
        static ComponentRegistry& GetInstance()
        {
            return *instance;
        }

        static void Init();
        static void Shutdown();

        template<typename ComponentType>
        static void RegisterComponent(const String& _name)
        {
            static_assert(std::is_base_of<Component, ComponentType>::value, "Template argument must inherit from Component!");
            ComponentRegistryFunctions funcs;
            funcs.createFunc = [&](GameEntity* _entity) { return _entity->AddComponent<ComponentType>(); };
            funcs.hasFunc = [&](GameEntity* entity) { return entity->HasComponent<ComponentType>(); };

            instance->registry[_name] = funcs;
        }

        static const HashMap<String, ComponentRegistryFunctions>& GetComponentMap()
        {
            return instance->registry;
        }

    private:
        HashMap<String, ComponentRegistryFunctions> registry;

        inline static ComponentRegistry* instance;
    };
}