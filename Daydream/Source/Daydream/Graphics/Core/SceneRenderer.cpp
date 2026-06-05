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

	RenderGraphDrawList SceneRenderer::CreateDrawListFromScene(Scene* _scene, const CameraData& _cameraData)
	{
		RenderGraphDrawList drawList{};

		Vector3 cameraPos = _cameraData.transform.position;
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

	void SceneRenderer::PrepareLighting(Scene* _scene, const CameraData& _cameraData)
	{
		lightData.dirLightCount = 0;
		lightData.pointLightCount = 0;
		lightData.spotLightCount = 0;
		lightData.eyePos = _cameraData.transform.position;
		for (EntityHandle handle : _scene->GetAllEntities())
		{
			GameEntity* entity = _scene->GetEntity(handle);
			if (!entity) continue;

			LightComponent* lightComponent = entity->GetComponent<LightComponent>();
			Transform transform = entity->GetComponent<TransformComponent>()->GetTransform();

			//TODO : Fix Later
			if (mainLightComponent == nullptr)
			{
				mainLightComponent = lightComponent;
				lightViewProj.viewMatrix = Matrix4x4::CreateLookToLH(transform.position, transform.GetForward(), transform.GetUp());
				lightViewProj.projectionMatrix = Matrix4x4::CreateOrthographicLH(-20.0f, 20.0f, -20.0f, 20.0f, -200.0f, 200.0f);
				lightViewProj.viewProjectionMatrix = lightViewProj.viewMatrix * lightViewProj.projectionMatrix;
				lightViewProj.viewMatrix.Transpose();
				lightViewProj.projectionMatrix.Transpose();
				lightViewProj.viewProjectionMatrix.Transpose();
			}
			if (lightComponent != nullptr)
			{
				Light light = lightComponent->GetLight();
				switch (light.type)
				{
				case Directional:
				{
					DirectionalLight dirLight;
					dirLight.color = light.color;
					dirLight.direction = transform.GetForward();
					dirLight.intensity = light.intensity;
					lightData.dirLights[lightData.dirLightCount++] = dirLight;
					break;
				}
				case Point:
				{
					PointLight pointLight;
					pointLight.color = light.color;
					pointLight.range = light.range;
					pointLight.intensity = light.intensity;
					pointLight.position = transform.position;
					lightData.pointLights[lightData.pointLightCount++] = pointLight;
					break;
				}
				case Spot:
				{
					SpotLight spotLight;
					spotLight.position = transform.position;
					spotLight.range = light.range;
					spotLight.direction = transform.GetForward();
					spotLight.intensity = light.intensity;
					spotLight.color = light.color;
					spotLight.innerConeCos = Math::CosDegree(light.spotInnerAngle);
					spotLight.outerConeCos = Math::CosDegree(light.spotOuterAngle);
					lightData.spotLights[lightData.spotLightCount++] = spotLight;
					break;
				}
				}
			}
		}
	}
}

