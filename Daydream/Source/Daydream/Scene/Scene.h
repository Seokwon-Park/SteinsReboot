#pragma once

#include "GameEntity/GameEntity.h"

#include "Daydream/Graphics/Camera/Camera.h"
#include "Daydream/Graphics/Resources/Light.h"

namespace Daydream
{

	class ModelRendererComponent;
	class LightComponent;
	class Skybox;

	struct LightData
	{
		DirectionalLight dirLights[2] = {};
		PointLight pointLights[4] = {};
		SpotLight spotLights[4] = {}; // 최대 32개 라이트
		Vector3 eyePos;
		UInt32 dirLightCount = 0;
		Vector2 padding1;
		UInt32 pointLightCount = 0;
		UInt32 spotLightCount = 0;
	};

	class Scene
	{
	public:
		Scene(const String& _name);
		~Scene();

		GameEntity* CreateGameEntity(const String& _name = "Entity");
		GameEntity* CreateGameEntityFromModel(AssetHandle _modelHandle);
		void DestroyEntity(EntityHandle _handle);
		GameEntity* GetEntity(EntityHandle _handle);
		const GameEntity* GetEntity(EntityHandle _handle) const;

		bool IsHandleValid(EntityHandle _handle) const;

		inline void SetCurrentCamera(Camera* _camera) { currentCamera = _camera; }
		inline const Camera* GetCurrentCamera() const { return currentCamera; }

		inline Shared<ConstantBuffer> GetLightConstantBuffer() const { return lightBuffer; }

		Skybox* GetSkybox() const;

		void Update(Float32 _deltaTime);
		//void RenderDepth();

		const Array<EntityHandle>& GetAllEntities() const { return activeEntities; }
		const Array<EntityHandle>& GetRootEntities() const { return rootEntities; }

		void AddRootEntity(EntityHandle _rootEntity);
		void RemoveRootEntity(EntityHandle _rootEntity);
		void ReorderRootEntity(EntityHandle _entityHandle, UInt64 _newIndex);

		//temp
		LightComponent* GetLightComponent() { return firstLightComponent; }
	private:
		void ProcessModelNode(GameEntity* _parentEntity, const NodeData& _curNode, const Model* _model);
		String name;

		Array<Unique<GameEntity>> entityPool;

		Array<UInt32> generations; //index의 generation
		Queue<UInt32> freeIndices; //사용가능한 인덱스

		Array<EntityHandle> activeEntities;
		Array<EntityHandle> rootEntities;

		Camera* currentCamera;

		Array<EntityHandle> lightEntities;
		Array<EntityHandle> modelRendererEntities;

		LightData lightData;
		Shared<ConstantBuffer> lightBuffer; 
		//Array<SpriteRendererComponent*> spriteRenderers;
		Skybox* skybox;

		//temp
		LightComponent* firstLightComponent = nullptr;
	};
}
