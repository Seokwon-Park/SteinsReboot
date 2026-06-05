#include "DaydreamPCH.h"
#include "DeferredSceneRenderer.h"

#include "Renderer.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"


namespace Daydream
{
	DeferredSceneRenderer::DeferredSceneRenderer()
	{
		lightViewProjectionBuffer = ConstantBuffer::Create(sizeof(ViewProjectionData));

		Texture2DDesc desc;
		desc.width = 1;
		desc.height = 1;
		desc.format = RenderFormat::R8G8B8A8_UNORM;
		desc.textureUsage = TextureUsage::RenderTarget | TextureUsage::ShaderResource;
		result = Texture2D::Create(desc);
	}

	DeferredSceneRenderer::~DeferredSceneRenderer()
	{
	}

	void DeferredSceneRenderer::RenderScene(const SceneData& _sceneData)
	{
		Renderer::UpdateConstantBuffer(lightViewProjectionBuffer, &lightViewProjData);

		UInt32 width = _sceneData.width;
		UInt32 height = _sceneData.height;

		if (result->GetWidth() != width || result->GetHeight() != height)
		{
			Texture2DPoolKey key{};
			key.width = result->GetWidth();
			key.height = result->GetHeight();
			key.format = result->GetFormat();

			Texture2DAllocation newAllocation{};
			newAllocation.texture = result;
			newAllocation.renderTargetView = resultRTV;

			Renderer::GetTexturePool()->ReturnResource(key, std::move(newAllocation));

			Texture2DDesc desc;
			desc.width = width;
			desc.height = height;
			desc.format = RenderFormat::R8G8B8A8_UNORM;
			desc.textureUsage = TextureUsage::RenderTarget | TextureUsage::ShaderResource;
			result = Texture2D::Create(desc);

			Renderer::TransitionTextureState(result.get(), ResourceState::Undefined, ResourceState::RenderTarget);
		}
		else
		{
			Renderer::TransitionTextureState(result.get(), ResourceState::ShaderResource, ResourceState::RenderTarget);
		}

		Scene* scene = _sceneData.scene;
		CameraData cameraData = _sceneData.cameraData;
		cameraData.viewProj.viewMatrix.Transpose();
		cameraData.viewProj.projectionMatrix.Transpose();
		cameraData.viewProj.viewProjectionMatrix.Transpose();

		PrepareLighting(scene, cameraData);

		renderGraph->Reset();

		RenderGraphDrawList opaqueDrawList = CreateDrawListFromScene(_sceneData.scene, cameraData);

		RenderGraphResourceDesc resourceDesc{};

		RenderGraphResourceHandle position = renderGraph->AddResource("PositionTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle albedo = renderGraph->AddResource("AlbedoTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle normal = renderGraph->AddResource("NormalTexture", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle rmao = renderGraph->AddResource("RMAOTexture", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle gbufferDepth = renderGraph->AddResource("GBufferDepth", { RenderFormat::R24G8_TYPELESS, width, height });
		RenderGraphResourceHandle shadowDepth = renderGraph->AddResource("DepthTexture", { RenderFormat::R24G8_TYPELESS, width, height });

		RenderGraphResourceHandle finalResource = renderGraph->AddExternalWriteResource("result", result);

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
		renderGraph->WriteDepthStencil(depthPass, shadowDepth);

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
		renderGraph->Write(gBufferPass, position);
		renderGraph->Write(gBufferPass, normal);
		renderGraph->Write(gBufferPass, albedo);
		renderGraph->Write(gBufferPass, rmao);
		renderGraph->WriteDepthStencil(gBufferPass, gbufferDepth);

		RenderGraphPassDesc deferredLightingPassDesc{};
		deferredLightingPassDesc.pipelineState = BuiltIn::PSO::Deferred();
		deferredLightingPassDesc.drawType = PassDrawType::FullScreenQuad;
		deferredLightingPassDesc.constantBufferData.push_back({ "LightViewProjection", &lightViewProj, sizeof(ViewProjectionData) });
		deferredLightingPassDesc.constantBufferData.push_back({ "Lights", &lightData, sizeof(SceneLightingData) });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "IrradianceTexture",scene->GetSkybox()->GetIrradianceTexture()->GetOrCreateDefaultSRV() });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "Prefilter",scene->GetSkybox()->GetPrefilterTexture()->GetOrCreateDefaultSRV() });
		deferredLightingPassDesc.shaderResourceViews.push_back({ "BRDFLUT", scene->GetSkybox()->GetBRDFTexture()->GetOrCreateDefaultSRV() });
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

		renderGraph->Read(lightingPass, position);
		renderGraph->Read(lightingPass, normal);
		renderGraph->Read(lightingPass, albedo);
		renderGraph->Read(lightingPass, rmao);
		renderGraph->Read(lightingPass, shadowDepth);
		renderGraph->Write(lightingPass, finalResource);

		renderGraph->Compile();
		renderGraph->Execute();

		Renderer::TransitionTextureState(result.get(), ResourceState::RenderTarget, ResourceState::ShaderResource);
	}
}

