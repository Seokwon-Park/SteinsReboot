#pragma once

#include "Component.h"
#include "Daydream/Graphics/Resources/Buffer.h"

namespace Daydream
{
	class TransformComponent : public Component
	{
	public:
		DEFINE_COMPONENT_NAME(TransformComponent);

		TransformComponent();
		virtual ~TransformComponent();

		virtual void Init() override;
		virtual void Update(Float32 _deltaTime) override;

		Matrix4x4 GetWorldMatrix();
		
		void SetTransform(Transform _transform);
		void SetTransform(Vector3 _position, Vector3 _rotation, Vector3 _scale);
		const Transform GetTransform() { return transform; }

		REFLECT_START()
			ADD_PROPERTY(FieldType::Transform, transform)
		REFLECT_END()
	private:
		Transform transform = Transform();

	};
}