#pragma once

#include "PropertyType.h"

#define DEFINE_COMPONENT_NAME(className)                            \
    const char* GetName() const override							\
	{																\
        return #className;                                          \
    }																			

namespace Daydream
{
	class GameEntity;

	class Component : public IReflectable
	{
	public:
		Component();
		virtual ~Component();

		inline void SetOwner(GameEntity* _entity) { owner = _entity; }
		inline GameEntity* GetOwner() const { return owner; }

		virtual void Init() = 0;
		virtual void Update(Float32 _deltaTime) = 0;
		
		virtual void OnPropertyChanged() = 0;

		virtual const char* GetName() const = 0;

	protected:
		GameEntity* owner = nullptr;
	};
}

