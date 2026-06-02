
#include "DaydreamPCH.h"
#include "Scene.h"

#include "Daydream/Asset/AssetManager.h"
#include "Components/LightComponent.h"
#include "Components/ModelRendererComponent.h"
#include "Components/MeshRendererComponent.h"
#include "Components/TransformComponent.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Resources/Skybox.h"

namespace Daydream
{
	Scene::Scene(const String& _name)
	{
		name = _name;

		skybox = Renderer::GetSkybox();

		entityPool.push_back(nullptr);
		generations.push_back(0); // 0번 인덱스의 generation은 0
	}

	Scene::~Scene()
	{
		
	}

	EntityHandle Scene::CreateGameEntity(const String& _name)
	{
		UInt32 index = 0;

		if (!freeIndices.empty()) // 남은 indices가 있으면
		{
			index = freeIndices.front();
			freeIndices.pop();
			entityPool[index] = MakeUnique<GameEntity>();
		}
		else
		{
			index = static_cast<UInt32>(entityPool.size());

			entityPool.push_back(MakeUnique<GameEntity>());
			generations.push_back(0);
		}
		UInt32 generation = generations[index];
		EntityHandle newHandle = EntityHandle::Create(index, generation);

		GameEntity* newEntity = entityPool[index].get();
		newEntity->SetHandle(newHandle);
		newEntity->SetScene(this);
		newEntity->SetName(_name);
		newEntity->Init();

		newEntity->AddComponent<TransformComponent>();

		activeEntities.push_back(newHandle);
		rootEntities.push_back(newHandle);

		return newHandle;
	}

	EntityHandle Scene::CreateGameEntityFromModel(AssetHandle _modelHandle)
	{
		Model* model = AssetManager::GetAsset<Model>(_modelHandle);
		if (model == nullptr)
		{
			return EntityHandle();
		}
		const ModelData* data = model->GetModelData();
		EntityHandle modelRootEntity = CreateGameEntity(data->rootNode.name);
		ProcessModelNode(modelRootEntity, data->rootNode, model);

		return modelRootEntity;
	}

	void Scene::DestroyEntity(EntityHandle _handle)
	{
		if (!IsHandleValid(_handle))
		{
			return; // 이미 파괴되었거나 유효하지 않은 핸들
		}

		GameEntity* entity = GetEntity(_handle);
		if (!entity)
		{
			return;
		}

		// 재귀 호출로 자식들을 먼저 파괴
		Array<EntityHandle> childrenCopy = entity->GetChildrenHandles();
		for (EntityHandle childHandle : childrenCopy)
		{
			DestroyEntity(childHandle); 
		}

		// 만약 부모가 있으면 이관계도 처리
		if (!entity->HasParent())
		{
			// 부모가 없었다면 최상위 객체였으므로 루트 목록에서 제거
			RemoveFromRootEntity(_handle);
		}

		UInt32 index = _handle.GetIndex();

		// 1. 엔티티 데이터 리셋 
		entityPool[index]->ResetSelf();

		// 2. 풀에서 엔티티 파괴 (UniquePtr이 nullptr이 됨)
		entityPool[index] = nullptr;

		// 3. Generation 증가 (이전 핸들을 모두 무효화)
		generations[index]++;

		// 4. 인덱스를 재사용 큐에 반환
		freeIndices.push(index);

		// 5. 모든 특수 목록에서 제거
		//removeHandleFromAllLists(_handle);

		// 6. 활성 목록에서 제거 O(N)
		activeEntities.erase(
			std::remove(activeEntities.begin(), activeEntities.end(), _handle),
			activeEntities.end()
		);
	}

	GameEntity* Scene::GetEntity(EntityHandle _handle)
	{
		if (!_handle.IsValid())
		{
			return nullptr;
		}
		return entityPool[_handle.GetIndex()].get();
	}

	const GameEntity* Scene::GetEntity(EntityHandle _handle) const
	{
		if (!_handle.IsValid())
		{
			return nullptr;
		}
		return entityPool[_handle.GetIndex()].get();
	}

	bool Scene::IsHandleValid(EntityHandle _handle) const
	{
		if (!_handle.IsValid()) // ID가 0인지 체크
		{
			return false;
		}

		UInt32 index = _handle.GetIndex();
		if (index >= generations.size()) // 인덱스 범위 체크
		{
			return false;
		}

		// 핸들의 generation과 슬롯의 generation이 일치하는지 확인
		return generations[index] == _handle.GetGeneration();
	}

	Skybox* Scene::GetSkybox() const
	{
		 return Renderer::GetSkybox(); 
	}

	void Scene::Update(Float32 _deltaTime)
	{
		for (EntityHandle handle : activeEntities)
		{
			GameEntity* entity = GetEntity(handle);
			if (entity)
			{
				entity->Update(_deltaTime);
			}
		}
	}

	void Scene::AddRootEntity(EntityHandle _rootEntity)
	{
		rootEntities.push_back(_rootEntity);
	}

	void Scene::RemoveFromRootEntity(EntityHandle _rootEntity)
	{
		rootEntities.erase(
			std::remove(rootEntities.begin(), rootEntities.end(), _rootEntity),
			rootEntities.end());
	}

	void Scene::ReorderRootEntity(EntityHandle _entityHandle, UInt64 _newIndex)
	{
		// 루트 목록에서 entityHandle을 찾음
		auto it = std::find(rootEntities.begin(), rootEntities.end(), _entityHandle);

		// 만약 루트로 옮기려는 entity가 root가 아닌 경우 일단 부모노드를 제거
		if (it == rootEntities.end())
		{
			GameEntity* entity = GetEntity(_entityHandle);
			if (entity)
			{
				entity->RemoveParent();
			}
			return;
		}

		// 현재 인덱스 계산
		UInt64 currentIndex = std::distance(rootEntities.begin(), it);

		// 같은 위치면 아무것도 안 함
		if (currentIndex == _newIndex)
		{
			return;
		}

		// 인덱스 범위 체크
		if (_newIndex > rootEntities.size())
		{
			_newIndex = rootEntities.size();
		}

		// 요소를 제거하고 새 위치에 삽입
		EntityHandle movedHandle = *it;
		rootEntities.erase(it);

		// erase 후 삽입 위치 조정
		if (_newIndex > currentIndex)
		{
			_newIndex--;
		}

		rootEntities.insert(rootEntities.begin() + _newIndex, movedHandle);
	}

	void Scene::ProcessModelNode(EntityHandle _parentEntityHandle, const NodeData& _curNode, const Model* _model)
	{
		String entityName = _curNode.name;
		if (entityName.empty()) entityName = "Node";

		auto modelData = _model->GetModelData();
		EntityHandle nodeEntityHandle = CreateGameEntity(entityName);
		GameEntity* nodeEntity = GetEntity(nodeEntityHandle);
		//nodeEntity->GetComponent<TransformComponent>()->SetTransform(_curNode.transform);

		// 부모 연결
		if (_parentEntityHandle.IsValid())
		{
			nodeEntity->SetParent(_parentEntityHandle);
		}

		// 트랜스폼 컴포넌트 가져와서 설정 (Matrix Decompose 등)
		TransformComponent* transform = nodeEntity->GetComponent<TransformComponent>();
		transform->SetTransform(_curNode.transform);

		// 3. 메쉬 처리 로직 (1개 vs N개)
		const Array<AssetHandle>& meshHandles = _model->GetMeshes();
		const Array<AssetHandle>& materialHandles = _model->GetMaterials(); 
		if (_curNode.meshIndex != -1)
		{
			UInt32 index = _curNode.meshIndex;
			AssetHandle meshHandle = meshHandles[index];
			AssetHandle materialHandle = materialHandles[modelData->meshes[index].materialIndex];

			MeshRendererComponent* meshRenderer = nodeEntity->AddComponent<MeshRendererComponent>();
			meshRenderer->SetMesh(meshHandle);
			meshRenderer->SetMaterial(materialHandle);
		}
		//else if (_curNode.meshIndices.size() > 1)
		//{
		//	// [Case B] 메쉬가 여러 개 -> 자식 엔티티(Sub-mesh)로 분리
		//	for (auto meshIndex : _curNode.meshIndices)
		//	{
		//		AssetHandle meshHandle = _model->GetMeshes()[meshIndex];
		//		AssetHandle materialHandle = _model->GetMaterials()[modelData->meshes[meshIndex].materialIndex];

		//		Shared<Mesh> meshAsset = AssetManager::GetAsset<Mesh>(meshHandle);

		//		// 이름 결정: "노드이름_메쉬이름" 혹은 그냥 "메쉬이름"
		//		String subName = meshAsset ? meshAsset->GetAssetName() : "SubMesh";

		//		// 주인(nodeEntity)의 자식으로 생성
		//		GameEntity* subEntity = CreateGameEntity(entityName + "_" + subName);
		//		subEntity->SetParent(nodeEntity->GetHandle());

		//		// 서브 엔티티의 트랜스폼은 Identity (부모인 nodeEntity가 위치를 잡고 있으므로)
		//		nodeEntity->GetComponent<TransformComponent>()->SetTransform(Transform());
		//		auto meshRenderer = subEntity->AddComponent<MeshRendererComponent>();
		//		meshRenderer->SetMesh(meshHandle);
		//		meshRenderer->SetMaterial(materialHandle);
		//		// materialHandle 설정...
		//	}
		//}

		// 재귀 자식 노드 순회 
		// 여기서 넘겨주는 부모는 방금 만든 nodeEntity
		for (const auto& childNode : _curNode.children)
		{
			ProcessModelNode(nodeEntityHandle, childNode, _model);
		}
	}
}