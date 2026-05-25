#include "DaydreamPCH.h"
#include "SceneRenderer.h"

#include "Daydream/Scene/Components/TransformComponent.h"
#include "Daydream/Scene/Components/LightComponent.h"
#include "Daydream/Scene/Components/MeshRendererComponent.h"
#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	SceneRenderer::SceneRenderer()
	{
		renderGraph = MakeShared<RenderGraph>();
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	RenderGraphDrawList SceneRenderer::CreateDrawListFromScene(Scene* _scene, Camera* _camera)
	{
		RenderGraphDrawList drawList{};

		Vector3 cameraPos = _camera->GetPosition();
		for (EntityHandle entityHandle : _scene->GetAllEntities())
		{
			GameEntity* entity = _scene->GetEntity(entityHandle);
			MeshRendererComponent* meshRenderer = entity->GetComponent<MeshRendererComponent>();

			TransformComponent* transform = entity->GetComponent<TransformComponent>();

			if (meshRenderer)
			{
				Mesh* mesh = AssetManager::GetAsset<Mesh>(meshRenderer->GetMeshHandle());
				Material* material = AssetManager::GetAsset<Material>(meshRenderer->GetMaterialHandle());

				if (mesh && material)
				{
					Float32 distanceToCamera = Vector3::Distance(cameraPos, transform->GetTransform().position);

					drawList.AddDrawObject(mesh, material, transform->GetWorldMatrix(), distanceToCamera);
				} 
			}
		}
		return drawList;
	}
}

