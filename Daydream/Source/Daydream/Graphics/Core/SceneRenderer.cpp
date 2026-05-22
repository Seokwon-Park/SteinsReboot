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

	Shared<RenderGraphDrawList> SceneRenderer::CreateDrawListFromScene(Scene* _scene, Camera* _camera)
	{
		Shared<RenderGraphDrawList> drawList = MakeShared<RenderGraphDrawList>();

		Vector3 cameraPos = _camera->GetPosition();
		for (EntityHandle entityHandle : _scene->GetAllEntities())
		{
			GameEntity* entity = _scene->GetEntity(entityHandle);
			MeshRendererComponent* meshRenderer = entity->GetComponent<MeshRendererComponent>();

			TransformComponent* transform = entity->GetComponent<TransformComponent>();

			if (meshRenderer)
			{
				Shared<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshRenderer->GetMeshHandle());
				Shared<Material> material = AssetManager::GetAsset<Material>(meshRenderer->GetMaterialHandle());

				if (mesh && material)
				{
					Float32 distanceToCamera = Vector3::Distance(cameraPos, transform->GetTransform().position);

					drawList->AddDrawObject(mesh, material, transform->GetWorldMatrix(), distanceToCamera);
				} 
			}
		}
		return drawList;
	}
}

