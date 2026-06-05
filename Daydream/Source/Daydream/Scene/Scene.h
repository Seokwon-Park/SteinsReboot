#pragma once

#include "GameEntity/GameEntity.h"

#include "Daydream/Graphics/Resources/Light.h"

namespace Daydream
{
	class ModelRendererComponent;
	class CameraComponent;
	class LightComponent;
	class Skybox;

	class Scene
	{
	public:
		Scene(const String& _name);
		~Scene();

		EntityHandle CreateGameEntity(const String& _name = "Entity");
		EntityHandle CreateGameEntityFromModel(AssetHandle _modelHandle);
		void DestroyEntity(EntityHandle _handle);

		GameEntity* GetEntity(EntityHandle _handle);
		const GameEntity* GetEntity(EntityHandle _handle) const;

		bool IsHandleValid(EntityHandle _handle) const;

		inline void SetCurrentCameraComponent(EntityHandle _camera) { cameraEntity = _camera; }
		inline const EntityHandle GetCurrentCamera() const { return cameraEntity; }

		Skybox* GetSkybox() const;

		void Update(Float32 _deltaTime);
		//void RenderDepth();

		const Array<EntityHandle>& GetAllEntities() const { return activeEntities; }
		const Array<EntityHandle>& GetRootEntities() const { return rootEntities; }

		void AddRootEntity(EntityHandle _rootEntity);
		void RemoveFromRootEntity(EntityHandle _rootEntity);

		//For Editor
		void ReorderRootEntity(EntityHandle _entityHandle, UInt64 _newIndex);

	private:
		EntityHandle ProcessModelNode(const NodeData& _curNode, const Array<AssetHandle>& _meshHandles, const Array<AssetHandle>& _matHandles);
		String name;

		Array<Unique<GameEntity>> entityPool;

		Array<UInt32> generations; //index의 generation
		Queue<UInt32> freeIndices; //사용가능한 인덱스

		Array<EntityHandle> activeEntities;
		Array<EntityHandle> rootEntities;

		EntityHandle cameraEntity;

		Skybox* skybox;

	};
}
