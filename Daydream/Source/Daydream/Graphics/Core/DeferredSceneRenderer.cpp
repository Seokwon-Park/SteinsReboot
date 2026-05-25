#include "DaydreamPCH.h"
#include "DeferredSceneRenderer.h"

#include "Renderer.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"


namespace Daydream
{
	DeferredSceneRenderer::DeferredSceneRenderer()
	{
	}

	DeferredSceneRenderer::~DeferredSceneRenderer()
	{
	}

	void DeferredSceneRenderer::RenderScene(const SceneData& _sceneData)
	{
		UInt32 width = _sceneData.width;
		UInt32 height = _sceneData.height;

		Scene* scene = _sceneData.scene;
		Camera* camera = _sceneData.camera;

		renderGraph->Reset();

		RenderGraphDrawList opaqueDrawList = CreateDrawListFromScene(_sceneData.scene, _sceneData.camera);

		RenderGraphResourceDesc resourceDesc{};

		RenderGraphResourceHandle position = renderGraph->AddResource("GBufferPos", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle normal = renderGraph->AddResource("GBufferNormal", { RenderFormat::R16G16B16A16_FLOAT, width, height });
		RenderGraphResourceHandle albedo = renderGraph->AddResource("GBufferAlbedo", { RenderFormat::R8G8B8A8_UNORM, width, height });
		RenderGraphResourceHandle mrao = renderGraph->AddResource("GBufferMRAO", { RenderFormat::R8G8B8A8_UNORM, width, height });

		RenderGraphResourceHandle shadowDepth = renderGraph->AddResource("Depth", { RenderFormat::D24_UNORM_S8_UINT, width, height });

		RenderGraphResourceHandle finalResource;

		RenderGraphPassDesc depthPassDesc{};
		depthPassDesc.pipelineState = BuiltIn::PSO::Depth();
		depthPassDesc.drawType = PassDrawType::DrawDepthStencil;
		depthPassDesc.drawList = opaqueDrawList;
		RenderGraphPassHandle depthPass = renderGraph->AddPass("DepthPass", depthPassDesc);
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
		renderGraph->Write(depthPass, shadowDepth);


		RenderGraphPassDesc gBufferPassDesc{};
		gBufferPassDesc.pipelineState = BuiltIn::PSO::GBuffer();
		gBufferPassDesc.drawType = PassDrawType::DrawMesh;
		gBufferPassDesc.drawList = opaqueDrawList;
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
		renderGraph->Write(gBufferPass, mrao);


		//RenderGraphPassHandle lightingPass = renderGraph->AddPass("DeferredLightingPass", [=]()
		//	{
		//		//Renderer::BeginRenderPass(renderPass, viewportFramebuffer);

		//		Renderer::BindPipelineState(PipelineStateRegistry::DeferredPSO);
		//		//Renderer::SetTextureView("PositionTexture", gBufferFramebuffer->GetColorAttachmentTexture(0));
		//		//Renderer::SetTextureView("NormalTexture", gBufferFramebuffer->GetColorAttachmentTexture(1));
		//		//Renderer::SetTextureView("AlbedoTexture", gBufferFramebuffer->GetColorAttachmentTexture(2));
		//		//Renderer::SetTextureView("RMAOTexture", gBufferFramebuffer->GetColorAttachmentTexture(3));
		//		//Renderer::SetTextureView("BRDFLUT", Renderer::GetSkybox()->GetBRDF());
		//		//Renderer::SetTextureView("EntityIDTexture", gBufferFramebuffer->GetColorAttachmentTexture(4));
		//		//Renderer::SetTextureView("OutlineTexture", maskFramebuffer->GetColorAttachmentTexture(0));
		//		//Renderer::SetTextureView("DepthTexture", depthFramebuffer->GetDepthAttachmentTexture());
		//		Renderer::BindConstantBuffer("Lights", activeScene->GetLightConstantBuffer());
		//		Renderer::BindConstantBuffer("EditorData", entityBuffer);
		//		//Renderer::SetTextureCube("IrradianceTexture", Renderer::GetSkybox()->GetIrradianceTexture());
		//		//Renderer::SetTextureCube("Prefilter", Renderer::GetSkybox()->GetPrefilterTexture());
		//		//deferredLightingMaterial->Bind();
		//		Renderer::BindMesh(ResourceManager::GetResource<Mesh>("Quad"));
		//		Renderer::DrawIndexed(ResourceManager::GetResource<Mesh>("Quad")->GetIndexCount());

		//		////pso3d->Bind();
		//		////activeScene->Update(_deltaTime);

		//		if (skyboxPanel->IsUsingSkybox())
		//		{
		//			Renderer::BindPipelineState(skyboxPipeline);
		//			Renderer::BindMesh(cubeMesh);
		//			Renderer::SetConstantBuffer("Camera", viewProjMat);
		//			//Renderer::SetTextureCube("TextureCubemap", activeScene->GetSkybox()->GetSkyboxTexture());
		//			Renderer::DrawIndexed(cubeMesh->GetIndexCount());
		//		}
		//		//Renderer::EndRenderPass(renderPass);
		//	});

		//renderGraph->Read(lightingPass, position);
		//renderGraph->Read(lightingPass, normal);
		//renderGraph->Read(lightingPass, albedo);
		//renderGraph->Write(lightingPass, finalResource);

		renderGraph->Compile();
		renderGraph->Execute();
	}
}

