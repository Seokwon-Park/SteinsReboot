#include "DaydreamPCH.h"
#include "SceneRenderer.h"

#include "Renderer.h"
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
			if (mainLightComponent == nullptr && lightComponent)
			{
				mainLightComponent = lightComponent;
			}

			if (lightComponent == mainLightComponent)
			{
				lightViewProj.viewMatrix = Matrix4x4::CreateLookToLH(-transform.GetForward()* 10.0f, transform.GetForward(), transform.GetUp());
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

	void SceneRenderer::ValidateResultTexture(UInt32 width, UInt32 height)
	{
		//만약 요구하는 크기와 현재 씬렌더러의 크기가 다르면
		if (result.texture->GetWidth() != width || result.texture->GetHeight() != height)
		{
			//현재의 텍스쳐는 풀에 던진다(어차피 사용안되면 없어짐)
			//Renderer::TransitionTextureState(result.texture, ResourceState::Undefined);
			Renderer::GetTexturePool()->ReturnAllocation(result);

			//크기에 맞춰서 새로운 텍스쳐를 생성한다.
			Texture2DDesc desc;
			desc.width = width;
			desc.height = height;
			desc.format = RenderFormat::R8G8B8A8_UNORM;
			desc.textureUsage = TextureUsage::RenderTarget | TextureUsage::ShaderResource;
			result.texture = Texture2D::Create(desc);

			TextureViewDesc rtvDesc{};
			rtvDesc.type = TextureViewType::RenderTarget;
			rtvDesc.baseMip = 0;
			rtvDesc.mipLevels = 1;
			rtvDesc.baseLayer = 0;
			rtvDesc.layerCount = 1;
			result.renderTargetView = TextureView::Create(result.texture, rtvDesc);

			//TextureViewDesc srvDesc{};
			//srvDesc.type = TextureViewType::ShaderResource;
			//srvDesc.baseMip = 0;
			//srvDesc.mipLevels = 1;
			//srvDesc.baseLayer = 0;
			//srvDesc.layerCount = 1;
			//result.shaderResourceView = TextureView::Create(result.texture, srvDesc);

			Renderer::TransitionTextureState(result.texture, ResourceState::RenderTarget);
		}
		else
		{
			Renderer::TransitionTextureState(result.texture, ResourceState::RenderTarget);
		}

		Renderer::TransitionTextureState(shadowMap.texture, ResourceState::DepthWrite);
	}
}

