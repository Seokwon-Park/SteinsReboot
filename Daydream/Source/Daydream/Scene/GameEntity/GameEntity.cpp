#include "DaydreamPCH.h"
#include "GameEntity.h"
#include "Daydream/Scene/Components/TransformComponent.h"
#include "Daydream/Scene/Scene.h"

namespace Daydream
{
	GameEntity::GameEntity()
	{
		
	}

	GameEntity::~GameEntity()
	{
	}

	void GameEntity::Init()
	{
	
	}

	void GameEntity::Update(Float32 _deltaTime)
	{
		//DAYDREAM_CORE_TRACE("{0} is Updated", name);
		for (auto& component : components)
		{
			component->Update(_deltaTime);
		}
	}
	//GameEntity* GameEntity::GetParent()
	//{
	//	if (!scene) return nullptr;

	//	return scene->GetEntity(parentHandle);
	//}

	bool GameEntity::IsDescendant(GameEntity* _target)
	{
		GameEntity* current = _target;
		while (current)
		{
			if (current == this) // 포인터 직접 비교
			{
				DAYDREAM_CORE_ERROR("[Scene] Cannot set descendant as parent - circular reference!");
				return true;
			}

			EntityHandle nextParentHandle = current->GetParentHandle();
			if (!nextParentHandle.IsValid())
			{
				break; // 루트 도달
			}

			current = scene->GetEntity(nextParentHandle);
			if (!current) // nullptr 체크
			{
				DAYDREAM_CORE_WARN("Parent chain broken - entity not found");
				return true;
			}
		}
		return false;
	}

	void GameEntity::SetParent(EntityHandle _parentHandle)
	{
		if (!scene)
		{
			DAYDREAM_CORE_ERROR("[Scene] Entity has no scene!");
			return;
		}

		if (handle == _parentHandle) // 부모가 자기 자신이 될 수 없음
		{
			DAYDREAM_CORE_ERROR("[Scene] Cannot set self as parent!");
			return;
		}

		if (parentHandle == _parentHandle) // 새로운 부모가 원래 부모와 같다면 바꿀필요가 없음
		{
			return;
		}

		GameEntity* newParent = nullptr;
		if (_parentHandle.IsValid()) // 새로운 부모의 핸들이 유효한지 확인
		{
			newParent = scene->GetEntity(_parentHandle);
			if (!newParent) // 새로운 부모의 핸들로 가져온 포인터가 유효한지 확인
			{
				DAYDREAM_CORE_ERROR("[Scene] Invalid parent handle - entity not found!");
				return; // 실패하면 아무것도 변경하지 않음
			}

			// 순환 참조 체크 (this의 새 부모 엔티티의 부모라인을 쭉 따라갔을 때 this를 부모로 두고 있으면 막음)
			GameEntity* checkParent = newParent;
			if (IsDescendant(checkParent))
			{
				return;
			}
		}

		//유효하지 않은 경우 newParent는 nullptr -> 다시말하면 부모 노드를 삭제하고싶다(RemoveParent)

		//만약 기존 부모노드가 있는경우 부모노드로부터 자신을 제거해줘야한다.
		if (HasParent())
		{
			GameEntity* oldParent = scene->GetEntity(parentHandle);
			if (oldParent)
			{
				oldParent->DetachChildInternal(handle); // oldParent의 자식 목록에서 'this' 제거
			}
		}
		else
		{
			// 기존 부모가 없었다면, 씬의 루트 엔티티였음
			scene->RemoveFromRootEntity(handle); // 씬의 루트 목록에서 'this' 제거
		}

		// this의 부모 핸들을 새 핸들로 갱신
		parentHandle = _parentHandle;

		auto transformComponent = GetComponent<TransformComponent>();
		// 새 부모에 this를 자식으로 추가
		if (newParent) // 새로운 부모가 있는경우
		{
			newParent->AddChildInternal(handle); // newParent의 자식 목록에 'this' 추가

			if (transformComponent)
			{
				if (newParent->HasComponent<TransformComponent>())
				{
					transformComponent->SetParent(newParent->GetComponent<TransformComponent>());
				}
				else
				{
					// 논리적 부모는 생겼지만 공간적(Transform) 부모가 없는 경우, 
					// 기존 트랜스폼 부모와의 연결은 끊음
					transformComponent->SetParent(nullptr);
				}
			}
		}
		else //없는경우
		{
			// _parentHandle이 유효하지 않은 경우(nullptr), 'this'는 루트 엔티티가 됨
			scene->AddRootEntity(handle); // 씬의 루트 목록에 'this' 추가
			if (transformComponent)
			{
				transformComponent->SetParent(nullptr);
			}
		}
	}

	void GameEntity::RemoveParent()
	{
		SetParent(EntityHandle());
	}

	void GameEntity::ResetSelf()
	{
		components.clear();
		componentMap.clear();
		scene = nullptr;
		parentHandle = EntityHandle();
		childrenHandles.clear();
		name = "";
		handle = EntityHandle();
	}

	void GameEntity::AddChildInternal(EntityHandle _childHandle)
	{
		if (!scene)
		{
			DAYDREAM_CORE_ERROR("Entity has no scene!");
			return;
		}

		if (handle == _childHandle)
		{
			DAYDREAM_CORE_WARN("An entity cannot be its own child.");
			return;
		}

		if (!_childHandle.IsValid())
		{
			DAYDREAM_CORE_WARN("Invalid child handle.");
			return;
		}

		auto itr = std::find(childrenHandles.begin(), childrenHandles.end(), _childHandle);
		if (itr != childrenHandles.end())
		{
			return;
		}

		GameEntity* newChild = scene->GetEntity(_childHandle);

		// 자식 엔티티 포인터가 유효한지 확인
		if (!newChild)
		{
			// 핸들은 유효(IsValid)했지만 Scene에서 엔티티를 찾지 못함 (예: 이미 파괴됨)
			DAYDREAM_CORE_ERROR("Child entity not found in scene!");
			return;
		}

		childrenHandles.push_back(_childHandle);
	}

	void GameEntity::DetachChildInternal(EntityHandle _childHandle)
	{
		// C++ 표준 라이브러리의 'erase-remove idiom' 
		// _childHandle이 아닌 값들을 앞쪽으로 shift 삭제할 값이 시작되는 위치를 return 
		// 모여 있는 삭제될 값들을 한번에 삭제

		childrenHandles.erase(
			std::remove(childrenHandles.begin(), childrenHandles.end(), _childHandle),
			childrenHandles.end()
		);
	}

	void GameEntity::ReorderChild(EntityHandle _childHandle, UInt64 _newIndex)
	{
		if (!scene)
		{
			DAYDREAM_CORE_ERROR("Entity has no scene!");
			return;
		}

		// 자식 목록에서 childHandle을 찾음
		auto it = std::find(childrenHandles.begin(), childrenHandles.end(), _childHandle);

		if (it == childrenHandles.end())
		{
			// 자식이 아니면 추가 후 순서 조정
			GameEntity* child = scene->GetEntity(_childHandle);
			if (child)
			{
				child->SetParent(handle);
			}
			return;
		}

		// 현재 인덱스 계산
		UInt64 currentIndex = std::distance(childrenHandles.begin(), it);

		// 같은 위치면 아무것도 안 함
		if (currentIndex == _newIndex)
		{
			return;
		}

		// 인덱스 범위 체크
		if (_newIndex > childrenHandles.size())
		{
			_newIndex = childrenHandles.size();
		}

		// 요소를 제거하고 새 위치에 삽입
		EntityHandle movedHandle = *it;
		childrenHandles.erase(it);

		// erase 후 삽입 위치 조정
		if (_newIndex > currentIndex)
		{
			_newIndex--;
		}

		childrenHandles.insert(childrenHandles.begin() + _newIndex, movedHandle);
	}
}
