#pragma once

#include "Component.h"

#include "Daydream/Graphics/Resources/Light.h"

namespace Daydream
{
	class LightComponent : public Component
	{
	public:
		DEFINE_COMPONENT_NAME(LightComponent);

		LightComponent();
		virtual ~LightComponent();

		virtual void Init() override;
		virtual void Update(Float32 _deltaTime) override;

		virtual void OnPropertyChanged() override {};
		const Light& GetLight() { return light; }

		REFLECT_START()
			ADD_PROPERTY(FieldType::Light, light)
		REFLECT_END()
	private:
		Light light;
	};
}