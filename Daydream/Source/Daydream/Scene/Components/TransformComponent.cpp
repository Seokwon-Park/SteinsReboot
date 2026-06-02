#include "DaydreamPCH.h"
#include "TransformComponent.h"

#include "Daydream/Scene/GameEntity/GameEntity.h"

namespace Daydream
{
	TransformComponent::TransformComponent()
	{
	}
	TransformComponent::~TransformComponent()
	{
	}
	void TransformComponent::Init()
	{
	}

	void TransformComponent::Update(Float32 _deltaTime)
	{

	}

	const Matrix4x4& TransformComponent::GetWorldMatrix() const
	{
		if (isDirty)
		{
			Matrix4x4 localMat = transform.GetLocalMatrix();

			if (parent)
			{
				worldMatrix = localMat * parent->GetWorldMatrix();
			}
			else
			{
				// 최상위 루트 오브젝트인 경우
				worldMatrix = localMat;
			}

			// 2. 갱신을 완료했으니 플래그를 끕니다.
			isDirty = false;
		}
		return worldMatrix;
	}

	void TransformComponent::SetParent(TransformComponent* _newParent)
	{
		if (parent)
		{
			parent->childs.erase(std::remove(parent->childs.begin(), parent->childs.end(), this), parent->childs.end());
		}

		parent = _newParent;

		if (parent)
		{
			parent->childs.push_back(this);
		}

		SetDirty();
	}

	void TransformComponent::SetDirty()
	{
		isDirty = true;

		for (auto child : childs)
		{
			child->SetDirty();
		}

	}
}
