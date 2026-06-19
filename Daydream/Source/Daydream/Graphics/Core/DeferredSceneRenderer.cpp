#include "DaydreamPCH.h"
#include "DeferredSceneRenderer.h"

#include "Renderer.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"
#include "Daydream/Asset/AssetManager.h"


namespace Daydream
{
	DeferredSceneRenderer::DeferredSceneRenderer()
	{
		lightViewProjectionBuffer = ConstantBuffer::Create(sizeof(ViewProjectionData));

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

	DeferredSceneRenderer::~DeferredSceneRenderer()
	{
	}

	void DeferredSceneRenderer::RenderScene(const SceneData& _sceneData)
	{
		Renderer::UpdateConstantBuffer(lightViewProjectionBuffer, &lightViewProjData);

		UInt32 width = _sceneData.width;
		UInt32 height = _sceneData.height;

		ValidateResultTexture(width, height);

		Scene* scene = _sceneData.scene;
		CameraData cameraData = _sceneData.cameraData;
		cameraData.viewProj.viewMatrix.Transpose();
		cameraData.viewProj.projectionMatrix.Transpose();
		cameraData.viewProj.viewProjectionMatrix.Transpose();

		PrepareLighting(scene, cameraData);

		renderGraph->Reset();

		RenderGraphDrawList opaqueDrawList = CreateDrawListFromScene(_sceneData.scene, cameraData);

		RenderGraphResourceDesc resourceDesc{};

		RenderGraphResourceHandle positionHandle = renderGraph->AddResource("rg_PositionTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle albedoHandle = renderGraph->AddResource("rg_AlbedoTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle normalHandle = renderGraph->AddResource("rg_NormalTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle rmaoHandle = renderGraph->AddResource("rg_RMAOTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle gbufferDepthHandle = renderGraph->AddResource("rg_GBufferDepth", { RenderFormat::R24G8_TYPELESS, width, height });
		RenderGraphResourceHandle shadowDepthHandle = renderGraph->AddExternalWriteResource("rg_DepthTexture", shadowMap);
		RenderGraphResourceHandle resultHandle = renderGraph->AddExternalWriteResource("Result", result);

		RenderGraphPassDesc shadowPassDesc{};
		shadowPassDesc.pipelineState = BuiltIn::PSO::Depth();
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
		gBufferPassDesc.pipelineState = BuiltIn::PSO::GBuffer();
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
		deferredLightingPassDesc.pipelineState = BuiltIn::PSO::Deferred();
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
		skyboxPassDesc.pipelineState = BuiltIn::PSO::EnvironmentCubemap();
		skyboxPassDesc.drawType = PassDrawType::DrawMesh;
		skyboxPassDesc.drawList.AddDrawObject(skyboxMesh);
		skyboxPassDesc.constantBufferData.push_back({ "Camera", &skyboxViewProjection, sizeof(ViewProjectionData) });
		skyboxPassDesc.shaderResourceViews.push_back({ "TextureCubemap", scene->GetSkybox()->GetSkyboxTexture()->GetDefaultSRV() });
		RenderGraphPassHandle skyboxPass = renderGraph->AddPass("SkyboxCubemapPass", skyboxPassDesc);
		renderGraph->AddPassDependency(lightingPass, skyboxPass);
		renderGraph->Write(skyboxPass, resultHandle, AttachmentLoadOp::Load);
		renderGraph->WriteDepthStencil(skyboxPass, gbufferDepthHandle, AttachmentLoadOp::Load);

		renderGraph->Compile();
		renderGraph->Execute();

		Renderer::TransitionTextureState(result.texture, ResourceState::ShaderResource);
		Renderer::TransitionTextureState(shadowMap.texture, ResourceState::ShaderResource);
	}

}

