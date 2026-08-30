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
		skyboxMesh = AssetManager::GetAsset<Mesh>(AssetDefaults::SkyboxSphereHandle);

		Texture2DDesc desc;
		desc.width = 1;
		desc.height = 1;
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

		desc.width = 2048;
		desc.height = 2048;
		desc.format = RenderFormat::R24G8_TYPELESS;
		desc.textureUsage = TextureUsage::DepthStencil | TextureUsage::ShaderResource;
		shadowMap.texture = Texture2D::Create(desc);

		TextureViewDesc dsvDesc{};
		dsvDesc.type = TextureViewType::DepthStencil;
		dsvDesc.baseMip = 0;
		dsvDesc.mipLevels = 1;
		dsvDesc.baseLayer = 0;
		dsvDesc.layerCount = 1;
		shadowMap.depthStencilView = TextureView::Create(shadowMap.texture, dsvDesc);

		//srvDesc.type = TextureViewType::ShaderResource;
		//srvDesc.format = RenderFormat::R24_UNORM_X8_TYPELESS;
		//srvDesc.baseMip = 0;
		//srvDesc.mipLevels = 1;
		//srvDesc.baseLayer = 0;
		//srvDesc.layerCount = 1;
		//shadowMap.shaderResourceView = TextureView::Create(shadowMap.texture, srvDesc);
	}

	SceneRenderer::~SceneRenderer()
	{
		Renderer::GetTexturePool()->ReturnAllocation(result);
		Renderer::GetTexturePool()->ReturnAllocation(shadowMap);
	}

	void SceneRenderer::RenderScene(const SceneData& _sceneData)
	{
		UInt32 width = _sceneData.width;
		UInt32 height = _sceneData.height;

		ValidateResultTexture(width, height);

		Scene* scene = _sceneData.scene;
		CameraData cameraData = _sceneData.cameraData;
		cameraData.viewProj.viewMatrix.Transpose();
		cameraData.viewProj.projectionMatrix.Transpose();
		cameraData.viewProj.viewProjectionMatrix.Transpose();

		PrepareLighting(scene, cameraData);

		RenderGraphDrawList opaqueDrawList = CreateDrawListFromScene(_sceneData.scene, cameraData);

		renderGraph->Reset();



		RenderGraphResourceDesc resourceDesc{};

		RenderGraphResourceHandle positionHandle = renderGraph->AddResource("PositionTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle albedoHandle = renderGraph->AddResource("AlbedoTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle normalHandle = renderGraph->AddResource("NormalTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle rmaoHandle = renderGraph->AddResource("RMAOTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle gbufferDepthHandle = renderGraph->AddResource("GBufferDepth", { RenderFormat::R24G8_TYPELESS, width, height });
		RenderGraphResourceHandle shadowDepthHandle = renderGraph->AddExternalWriteResource("DepthTexture", shadowMap);
		RenderGraphResourceHandle resultHandle = renderGraph->AddExternalWriteResource("Result", result);

		RenderGraphPassDesc shadowPassDesc{};
		shadowPassDesc.psoDesc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::DepthPipelineHandle));
		shadowPassDesc.drawType = PassDrawType::DrawDepthStencil;
		shadowPassDesc.drawList = opaqueDrawList;
		shadowPassDesc.constantBufferData.push_back({ "LightViewProjection", &lightViewProj, sizeof(ViewProjectionData) });

		RenderGraphPassHandle depthPass = renderGraph->AddPass("ShadowPath", shadowPassDesc);
		//RenderGraphPassHandle depthPass = renderGraph->AddPass("DepthPass", [=]()
		//	{

		//		Renderer::BindPipelineState(PipelineStateRegistry::DepthPSO);
		//		if (scene->GetLightComponent())
		//		{
		//			Renderer::BindConstantBuffer("LightSpace", scene->GetLightComponent()->GetLight().lightViewProjectionBuffer);
		//		}
		//		for (const auto entityHandle : scene->GetAllEntities())
		//		{
		//			GameEntity* entity = scene->GetEntity(entityHandle);

		//			MeshRendererComponent* meshRenderer = entity->GetComponent<MeshRendererComponent>();
		//			if (meshRenderer)
		//			{
		//				Renderer::BindConstantBuffer("World", meshRenderer->GetWorldMatrixConstantBuffer());
		//				auto mesh = AssetManager::GetAsset<Mesh>(meshRenderer->GetMesh());
		//				if (mesh)
		//				{
		//					Renderer::BindMesh(mesh);
		//					Renderer::DrawIndexed(mesh->GetIndexCount());
		//				}
		//			}
		//		}
		//		//Renderer::EndRenderPass(depthRenderPass);
		//	});
		renderGraph->WriteDepthStencil(depthPass, shadowDepthHandle);

		RenderGraphPassDesc gBufferPassDesc{};
		gBufferPassDesc.psoDesc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::GBufferPipelineHandle));
		gBufferPassDesc.drawType = PassDrawType::DrawMesh;
		gBufferPassDesc.drawList = opaqueDrawList;
		gBufferPassDesc.constantBufferData.push_back({ "Camera", &cameraData.viewProj, sizeof(ViewProjectionData) });
		RenderGraphPassHandle gBufferPass = renderGraph->AddPass("GBufferPass", gBufferPassDesc);
		//	{
		//		//Renderer::Submit(squareIB->GetCount());
		//		//Renderer::BeginRenderPass(gBufferRenderPass, gBufferFramebuffer);
		//		Renderer::BindPipelineState(PipelineStateRegistry::GBufferPSO);
		//		Renderer::BindConstantBuffer("Camera", camera->GetViewProjectionConstantBuffer());
		//		for (const auto entityHandle : scene->GetAllEntities())
		//		{
		//			GameEntity* entity = scene->GetEntity(entityHandle);
		//			MeshRendererComponent* meshRenderer = entity->GetComponent<MeshRendererComponent>();
		//			if (meshRenderer == nullptr) continue;
		//			Renderer::BindConstantBuffer("World", meshRenderer->GetWorldMatrixConstantBuffer());
		//			Renderer::BindConstantBuffer("Entity", meshRenderer->GetEntityHandleConstantBuffer());
		//			auto mesh = AssetManager::GetAsset<Mesh>(meshRenderer->GetMesh());
		//			auto material = AssetManager::GetAsset<Material>(meshRenderer->GetMaterial());
		//			if (mesh && material)
		//			{
		//				Renderer::BindMesh(mesh);
		//				Renderer::BindMaterial(material);
		//				Renderer::DrawIndexed(mesh->GetIndexCount());
		//			}
		//		}
		//	});
		renderGraph->Write(gBufferPass, positionHandle);
		renderGraph->Write(gBufferPass, normalHandle);
		renderGraph->Write(gBufferPass, albedoHandle);
		renderGraph->Write(gBufferPass, rmaoHandle);
		renderGraph->WriteDepthStencil(gBufferPass, gbufferDepthHandle);

		RenderGraphPassDesc deferredLightingPassDesc{};
		deferredLightingPassDesc.psoDesc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::DeferredPBRPipelineHandle));
		deferredLightingPassDesc.drawType = PassDrawType::FullScreenQuad;
		deferredLightingPassDesc.constantBufferData.push_back({ "LightViewProjection", &lightViewProj, sizeof(ViewProjectionData) });
		deferredLightingPassDesc.constantBufferData.push_back({ "Lights", &lightData, sizeof(SceneLightingData) });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "IrradianceTexture",scene->GetSkybox()->GetIrradianceTexture()->GetDefaultSRV() });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "Prefilter",scene->GetSkybox()->GetPrefilterTexture()->GetDefaultSRV() });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "BRDFLUT", scene->GetSkybox()->GetBRDFTexture()->GetDefaultSRV() });
		RenderGraphPassHandle lightingPass = renderGraph->AddPass("DeferredLightingPass", deferredLightingPassDesc);
		//	//{
		//	//	//Renderer::BeginRenderPass(renderPass, viewportFramebuffer);

		//	//	Renderer::BindPipelineState(PipelineStateRegistry::DeferredPSO);
		//	//	//Renderer::SetTextureView("PositionTexture", gBufferFramebuffer->GetColorAttachmentTexture(0));
		//	//	//Renderer::SetTextureView("NormalTexture", gBufferFramebuffer->GetColorAttachmentTexture(1));
		//	//	//Renderer::SetTextureView("AlbedoTexture", gBufferFramebuffer->GetColorAttachmentTexture(2));
		//	//	//Renderer::SetTextureView("RMAOTexture", gBufferFramebuffer->GetColorAttachmentTexture(3));
		//	//	//Renderer::SetTextureView("BRDFLUT", Renderer::GetSkybox()->GetBRDF());
		//	//	//Renderer::SetTextureView("EntityIDTexture", gBufferFramebuffer->GetColorAttachmentTexture(4));
		//	//	//Renderer::SetTextureView("OutlineTexture", maskFramebuffer->GetColorAttachmentTexture(0));
		//	//	//Renderer::SetTextureView("DepthTexture", depthFramebuffer->GetDepthAttachmentTexture());
		//	//	Renderer::BindConstantBuffer("Lights", activeScene->GetLightConstantBuffer());
		//	//	Renderer::BindConstantBuffer("EditorData", entityBuffer);
		//	//	//Renderer::SetTextureCube("IrradianceTexture", Renderer::GetSkybox()->GetIrradianceTexture());
		//	//	//Renderer::SetTextureCube("Prefilter", Renderer::GetSkybox()->GetPrefilterTexture());
		//	//	//deferredLightingMaterial->Bind();
		//	//	Renderer::BindMesh(ResourceManager::GetResource<Mesh>("Quad"));
		//	//	Renderer::DrawIndexed(ResourceManager::GetResource<Mesh>("Quad")->GetIndexCount());

		//	//	////pso3d->Bind();
		//	//	////activeScene->Update(_deltaTime);

		//	//	if (skyboxPanel->IsUsingSkybox())
		//	//	{
		//	//		Renderer::BindPipelineState(skyboxPipeline);
		//	//		Renderer::BindMesh(cubeMesh);
		//	//		Renderer::SetConstantBuffer("Camera", viewProjMat);
		//	//		//Renderer::SetTextureCube("TextureCubemap", activeScene->GetSkybox()->GetSkyboxTexture());
		//	//		Renderer::DrawIndexed(cubeMesh->GetIndexCount());
		//	//	}
		//	//	//Renderer::EndRenderPass(renderPass);
		//	//});

		renderGraph->Read(lightingPass, positionHandle);
		renderGraph->Read(lightingPass, normalHandle);
		renderGraph->Read(lightingPass, albedoHandle);
		renderGraph->Read(lightingPass, rmaoHandle);
		renderGraph->Read(lightingPass, shadowDepthHandle);
		renderGraph->Write(lightingPass, resultHandle);

		ViewProjectionData skyboxViewProjection = _sceneData.cameraData.viewProj;

		skyboxViewProjection.viewMatrix[3][0] = 0.0f; // X 이동 제거
		skyboxViewProjection.viewMatrix[3][1] = 0.0f; // Y 이동 제거
		skyboxViewProjection.viewMatrix[3][2] = 0.0f; // Z 이동 제거

		skyboxViewProjection.viewProjectionMatrix = skyboxViewProjection.viewMatrix * skyboxViewProjection.projectionMatrix;

		skyboxViewProjection.viewMatrix.Transpose();
		skyboxViewProjection.projectionMatrix.Transpose();
		skyboxViewProjection.viewProjectionMatrix.Transpose();

		RenderGraphPassDesc skyboxPassDesc{};
		skyboxPassDesc.psoDesc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::EnvironmentPipelineHandle));
		skyboxPassDesc.drawType = PassDrawType::DrawMesh;
		skyboxPassDesc.drawList.AddDrawObject(skyboxMesh);
		skyboxPassDesc.constantBufferData.push_back({ "Camera", &skyboxViewProjection, sizeof(ViewProjectionData) });
		skyboxPassDesc.shaderResourceViews.push_back({ "TextureCubemap", scene->GetSkybox()->GetSkyboxTexture()->GetDefaultSRV() });
		RenderGraphPassHandle skyboxPass = renderGraph->AddPass("SkyboxCubemapPass", skyboxPassDesc);
		renderGraph->AddPassDependency(lightingPass, skyboxPass);
		renderGraph->Write(skyboxPass, resultHandle, AttachmentLoadOp::Load);
		renderGraph->WriteDepthStencil(skyboxPass, gbufferDepthHandle, AttachmentLoadOp::Load);



		Renderer::TransitionTextureState(result.texture, ResourceState::ShaderResource);
		Renderer::TransitionTextureState(shadowMap.texture, ResourceState::ShaderResource);

		renderGraph->Compile();
		renderGraph->Execute();
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
				lightViewProj.viewMatrix = Matrix4x4::CreateLookToLH(-transform.GetForward() * 10.0f, transform.GetForward(), transform.GetUp());
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

