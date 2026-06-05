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

		virtual void OnPropertyChanged() override;

		const Matrix4x4& GetWorldMatrix() const;

		 void SetParent(TransformComponent* _newParent);

		inline void SetTransform(const Transform& _transform) 
		{ 
			transform = _transform; 
			isDirty = true;
		}

		inline void SetTransform(Vector3 _position, Vector3 _rotation, Vector3 _scale)
		{
			transform.position = _position;
			transform.rotation = _rotation;
			transform.scale = _scale;
			isDirty = true;
		}
		Vector3 GetPosition() const { return transform.position; }
		Vector3 GetRotation() const { return transform.rotation; }
		Vector3 GetScale() const { return transform.scale; }
		Vector3 GetForward() const { return transform.GetForward(); }
		Vector3 GetUp() const { return transform.GetUp(); }
		Vector3 GetRight() const { return transform.GetRight(); }
		const Transform& GetTransform() const { return transform; }

		REFLECT_START()
			ADD_PROPERTY(FieldType::Transform, transform)
			REFLECT_END()
	private:
		void SetDirty();

		Transform transform = Transform();
		mutable Matrix4x4 worldMatrix = Matrix4x4();
		mutable bool isDirty = false;

		TransformComponent* parent = nullptr;
		Array<TransformComponent*> childs;
	};
}