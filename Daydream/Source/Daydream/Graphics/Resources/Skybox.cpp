#include "DaydreamPCH.h"
#include "Skybox.h"

#include "Daydream/Asset/AssetManager.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Cache/GraphicsCacheRegistry.h"
#include "Daydream/Graphics/Utility/MeshGenerator.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"

namespace Daydream
{
	Skybox::Skybox()
	{
		skyboxCaptureViewProjection[0].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		skyboxCaptureViewProjection[1].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		skyboxCaptureViewProjection[2].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
		skyboxCaptureViewProjection[3].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
		skyboxCaptureViewProjection[4].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
		skyboxCaptureViewProjection[5].viewMatrix = Matrix4x4::CreateLookToLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

		Matrix4x4 projection = Matrix4x4::CreatePerspectiveLH(Math::DegreeToRadian(90.0f), 1.0f, 0.1f, 10.0f);
		cubeFaceConstantBuffers.resize(6);
		for (int i = 0; i < 6; i++)
		{
			skyboxCaptureViewProjection[i].projectionMatrix = projection;
			skyboxCaptureViewProjection[i].viewProjectionMatrix = skyboxCaptureViewProjection[i].viewMatrix * skyboxCaptureViewProjection[i].projectionMatrix;
			skyboxCaptureViewProjection[i].viewMatrix.Transpose();
			skyboxCaptureViewProjection[i].projectionMatrix.Transpose();
			skyboxCaptureViewProjection[i].viewProjectionMatrix.Transpose();
			cubeFaceConstantBuffers[i] = ConstantBuffer::Create(sizeof(ViewProjectionData));
		}
	}

	Skybox::~Skybox()
	{
		//oldTextures.clear();
		//skyboxTextureCube = nullptr;
		equirectangularTexture = nullptr;
		skyboxFaceRTVs.clear();
		//captureFramebuffer = nullptr;
		//equirectangularResultTextures.clear();

		/*equirectangularMaterials.clear();
		irradianceMaterials.clear();
		prefilterMaterials.clear();
		resizeMaterial = nullptr;*/
	}

	void Skybox::CreateResources()
	{
		GraphicsPipelineStateDesc desc;
		desc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::EquirectangularPipelineHandle));
		desc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };

		equirectangularPSO = GraphicsCacheRegistry::RequestPipelineState(desc);

		desc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::ResizePipelineHandle));
		desc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		resizePSO = GraphicsCacheRegistry::RequestPipelineState(desc);

		desc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::IrradiancePipelineHandle));
		desc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		irradiancePSO = GraphicsCacheRegistry::RequestPipelineState(desc);

		desc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::PrefilterPipelineHandle));
		desc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		prefilterPSO = GraphicsCacheRegistry::RequestPipelineState(desc);

		equirectangularDropTarget = AssetManager::GetAssetByPath<Texture2D>("Resource/NoTexture.png");

		desc.InitWithShaderPipeline(AssetManager::GetAsset<ShaderPipeline>(AssetDefaults::BRDFPipelineHandle));
		desc.renderTargetFormats = { RenderFormat::R16G16B16A16_FLOAT };
		brdfPSO = GraphicsCacheRegistry::RequestPipelineState(desc);

		quadMesh = AssetManager::GetAsset<Mesh>(AssetDefaults::QuadMeshHandle);
		boxMesh = AssetManager::GetAsset<Mesh>(AssetDefaults::BoxMeshHandle);

		////////////////////////////////////////////////////////////////////////////Create Default Skybox TextureCubes;
		skyboxMipLevels = (UInt32)std::log2f((Float32)skyboxResolution);

		Texture2DDesc textureDesc{};
		textureDesc.width = skyboxResolution;
		textureDesc.height = skyboxResolution;
		textureDesc.mipLevels = skyboxMipLevels;
		textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
		textureDesc.format = RenderFormat::R16G16B16A16_FLOAT;
		skyboxTextureCube = TextureCube::Create(textureDesc);

		TextureViewDesc rtvDesc{};
		TextureViewDesc srvDesc{};
		TextureViewDesc dsvDesc{};
		skyboxFaceRTVs.resize(6);
		skyboxFaceSRVs.resize(6);
		for (UInt32 i = 0; i < 6; i++)
		{
			rtvDesc.type = TextureViewType::RenderTarget;
			rtvDesc.baseMip = 0;
			rtvDesc.mipLevels = 1;
			rtvDesc.baseLayer = i;
			rtvDesc.layerCount = 1;
			skyboxFaceRTVs[i] = TextureView::Create(skyboxTextureCube, rtvDesc);

			srvDesc.type = TextureViewType::ShaderResource;
			srvDesc.baseMip = 0;
			srvDesc.mipLevels = 1;
			srvDesc.baseLayer = i;
			srvDesc.layerCount = 1;
			skyboxFaceSRVs[i] = TextureView::Create(skyboxTextureCube, srvDesc);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////Create Irradiance TextureCube

		textureDesc.width = diffuseResolution;
		textureDesc.height = diffuseResolution;
		textureDesc.mipLevels = 1;
		textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
		textureDesc.format = RenderFormat::R16G16B16A16_FLOAT;
		irradianceTextureCube = TextureCube::Create(textureDesc);

		irradianceRTVs.resize(6);
		irradianceSRVs.resize(6);
		for (UInt32 i = 0; i < 6; i++)
		{
			rtvDesc.type = TextureViewType::RenderTarget;
			rtvDesc.baseMip = 0;
			rtvDesc.mipLevels = 1;
			rtvDesc.baseLayer = i;
			rtvDesc.layerCount = 1;

			irradianceRTVs[i] = TextureView::Create(irradianceTextureCube, rtvDesc);

			srvDesc.type = TextureViewType::ShaderResource;
			srvDesc.baseMip = 0;
			srvDesc.mipLevels = 1;
			srvDesc.baseLayer = i;
			srvDesc.layerCount = 1;
			irradianceSRVs[i] = TextureView::Create(irradianceTextureCube, srvDesc);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////Create Prefilter TextureCube

		prefilterMipLevels = (UInt32)std::log2f((Float32)specularResolution);

		textureDesc.width = specularResolution;
		textureDesc.height = specularResolution;
		textureDesc.mipLevels = prefilterMipLevels;
		textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
		textureDesc.format = RenderFormat::R16G16B16A16_FLOAT;

		prefilterTextureCube = TextureCube::Create(textureDesc);


		prefilterRTVs.resize(6 * prefilterMipLevels);
		prefilterSRVs.resize(6 * prefilterMipLevels);
		for (UInt32 mip = 0; mip < prefilterMipLevels; mip++)
		{
			for (UInt32 face = 0; face < 6; face++)
			{
				UInt32 index = prefilterMipLevels * face + mip;


				rtvDesc.type = TextureViewType::RenderTarget;
				rtvDesc.baseMip = mip;
				rtvDesc.mipLevels = 1;
				rtvDesc.baseLayer = face;
				rtvDesc.layerCount = 1;

				prefilterRTVs[index] = TextureView::Create(prefilterTextureCube, rtvDesc);

				srvDesc.type = TextureViewType::ShaderResource;
				srvDesc.baseMip = mip;
				srvDesc.mipLevels = 1;
				srvDesc.baseLayer = face;
				srvDesc.layerCount = 1;
				prefilterSRVs[index] = TextureView::Create(prefilterTextureCube, srvDesc);
			}
		}

		roughnessConstantBuffers.resize(prefilterMipLevels);
		for (UInt32 mip = 0; mip < prefilterMipLevels; mip++)
		{
			roughnessConstantBuffers[mip] = ConstantBuffer::Create(sizeof(Vector4));
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////////BRDF

		textureDesc.width = skyboxTextureCube->GetWidth();
		textureDesc.height = skyboxTextureCube->GetHeight();
		textureDesc.mipLevels = 1;
		textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
		textureDesc.format = RenderFormat::R16G16B16A16_FLOAT;
		BRDFTexture = Texture2D::Create(textureDesc);

		rtvDesc.type = TextureViewType::RenderTarget;
		rtvDesc.baseMip = 0;
		rtvDesc.mipLevels = 1;
		rtvDesc.baseLayer = 0;
		rtvDesc.layerCount = 1;
		BRDFRTV = TextureView::Create(BRDFTexture, rtvDesc);

		srvDesc.type = TextureViewType::ShaderResource;
		srvDesc.baseMip = 0;
		srvDesc.mipLevels = 1;
		srvDesc.baseLayer = 0;
		srvDesc.layerCount = 1;
		BRDFSRV = TextureView::Create(BRDFTexture, srvDesc);


		//////////////////////////////////////Create Resize Texture

		textureDesc.width = skyboxTextureCube->GetWidth();
		textureDesc.height = skyboxTextureCube->GetHeight();
		textureDesc.mipLevels = 1;
		textureDesc.textureUsage = Daydream::TextureUsage::ShaderResource | TextureUsage::RenderTarget;
		textureDesc.format = RenderFormat::R16G16B16A16_FLOAT;

		resizeTexture = Texture2D::Create(textureDesc);

		rtvDesc.type = TextureViewType::RenderTarget;
		rtvDesc.baseMip = 0;
		rtvDesc.mipLevels = 1;
		rtvDesc.baseLayer = 0;
		rtvDesc.layerCount = 1;
		resizeRTV = TextureView::Create(resizeTexture, rtvDesc);

		equirectangularTexture = AssetManager::GetAssetByPath<Texture2D>("Resource/skybox.hdr");
	}

	void Skybox::GenerateDefault()
	{
		Renderer::TransitionTextureState(skyboxTextureCube, ResourceState::RenderTarget);
		Renderer::TransitionTextureState(irradianceTextureCube, ResourceState::RenderTarget);
		Renderer::TransitionTextureState(prefilterTextureCube, ResourceState::RenderTarget);
		Renderer::TransitionTextureState(BRDFTexture, ResourceState::RenderTarget);

		for (int i = 0; i < 6; i++)
		{
			Renderer::UpdateConstantBuffer(cubeFaceConstantBuffers[i].get(), skyboxCaptureViewProjection[i]);
		}

		GenerateHDRCubemap(equirectangularTexture);

		Renderer::TransitionTextureState(skyboxTextureCube, ResourceState::ShaderResource);

		GenerateBRDF();
		GenerateIrradianceCubemap();
		GeneratePrefilterCubemap();


		Renderer::TransitionTextureState(irradianceTextureCube, ResourceState::ShaderResource);
		Renderer::TransitionTextureState(prefilterTextureCube, ResourceState::ShaderResource);
		Renderer::TransitionTextureState(BRDFTexture, ResourceState::ShaderResource);
	}

	void Skybox::Update()
	{
		//if (oldTextures.size() > 200)
		//{
		//	DAYDREAM_CORE_INFO("Clear Old SkyboxTextures");
		//	oldTextures.clear();
		//}
	}

	void Skybox::GenerateHDRCubemap(Texture2D* _texture)
	{
		boxMesh->GetIndexCount();
		equirectangularTexture = _texture;

		for (int i = 0; i < 6; i++)
		{
			RenderingInfo renderingInfo{};

			renderingInfo.renderArea.x = 0;
			renderingInfo.renderArea.y = 0;
			renderingInfo.renderArea.width = skyboxResolution;
			renderingInfo.renderArea.height = skyboxResolution;

			AttachmentDesc attachDesc{};
			attachDesc.view = skyboxFaceRTVs[i].get();

			renderingInfo.colorAttachments.push_back(attachDesc);

			Renderer::BeginRendering(renderingInfo);
			Renderer::BindPipelineState(equirectangularPSO);
			Renderer::BindConstantBuffer("Camera", cubeFaceConstantBuffers[i]);
			Renderer::BindShaderResourceView("Texture", equirectangularTexture);
			Renderer::BindMesh(boxMesh);
			Renderer::DrawIndexed(boxMesh->GetIndexCount());
			Renderer::EndRendering(renderingInfo);
		}
		Renderer::GenerateMips(skyboxTextureCube);
	}

	void Skybox::GenerateIrradianceCubemap()
	{
		for (int i = 0; i < 6; i++)
		{
			RenderingInfo renderingInfo{};

			renderingInfo.renderArea.x = 0;
			renderingInfo.renderArea.y = 0;
			renderingInfo.renderArea.width = diffuseResolution;
			renderingInfo.renderArea.height = diffuseResolution;

			AttachmentDesc attachDesc{};
			attachDesc.view = irradianceRTVs[i].get();

			renderingInfo.colorAttachments.push_back(attachDesc);

			Renderer::BeginRendering(renderingInfo);
			Renderer::BindPipelineState(irradiancePSO);
			Renderer::BindConstantBuffer("Camera", cubeFaceConstantBuffers[i]);
			Renderer::BindShaderResourceView("TextureCubemap", skyboxTextureCube);
			Renderer::BindMesh(boxMesh);
			Renderer::DrawIndexed(boxMesh->GetIndexCount());
			Renderer::EndRendering(renderingInfo);
		}
	}

	void Skybox::GeneratePrefilterCubemap()
	{
		for (UInt32 mip = 0; mip < prefilterMipLevels; mip++)
		{
			roughness = Vector4(0.0f, 0.0f, 0.0f, (float)mip / (prefilterMipLevels - 1));
			Renderer::UpdateConstantBuffer(roughnessConstantBuffers[mip], roughness);

			RenderArea renderArea;
			renderArea.x = 0;
			renderArea.y = 0;
			renderArea.width = Math::Max(1U, specularResolution >> mip);
			renderArea.height = Math::Max(1U, specularResolution >> mip);

			for (int face = 0; face < 6; face++)
			{
				UInt32 index = prefilterMipLevels * face + mip;

				RenderingInfo renderingInfo{};

				renderingInfo.renderArea = renderArea;

				AttachmentDesc attachDesc{};
				attachDesc.view = prefilterRTVs[index].get();

				renderingInfo.colorAttachments.push_back(attachDesc);

				Renderer::BeginRendering(renderingInfo);
				Renderer::BindPipelineState(prefilterPSO);
				Renderer::BindConstantBuffer("Camera", cubeFaceConstantBuffers[face]);
				Renderer::BindConstantBuffer("Roughness", roughnessConstantBuffers[mip]);
				Renderer::BindShaderResourceView("TextureCubemap", skyboxTextureCube);
				Renderer::BindMesh(boxMesh);
				Renderer::DrawIndexed(boxMesh->GetIndexCount());
				Renderer::EndRendering(renderingInfo);
			}
		}
	}

	void Skybox::GenerateBRDF()
	{
		RenderingInfo renderingInfo{};

		renderingInfo.renderArea.x = 0;
		renderingInfo.renderArea.y = 0;
		renderingInfo.renderArea.width = skyboxResolution;
		renderingInfo.renderArea.height = skyboxResolution;

		AttachmentDesc attachDesc{};
		attachDesc.view = BRDFRTV.get();

		renderingInfo.colorAttachments.push_back(attachDesc);
		Renderer::BeginRendering(renderingInfo);
		Renderer::BindPipelineState(brdfPSO);
		Renderer::BindMesh(quadMesh);
		Renderer::DrawIndexed(quadMesh->GetIndexCount());
		Renderer::EndRendering(renderingInfo);
	}

	void Skybox::UpdateSkyboxFace(UInt32 _faceIndex, Shared<Texture2D> _texture)
	{
		//Renderer::BeginRenderPass(resizeRenderPass, resizeFramebuffer);
		//Renderer::BindPipelineState(resizePSO);
		//Renderer::SetTexture2D("Texture", _texture);
		//Renderer::BindMesh(quadMesh);
		//Renderer::DrawIndexed(quadMesh->GetIndexCount());
		//Renderer::EndRenderPass(resizeRenderPass);

		////Renderer::CopyTexture2D(resizeFramebuffer->GetColorAttachmentTexture(0), resizeResultTexture);
		//Renderer::CopyTextureToCubemapFace(skyboxTextureCube, _faceIndex, resizeResultTexture, 0);
		//skyboxTextureCube->Update(_faceIndex, resizeResultTexture);
		////oldTextures.push_back(resizeResultTexture);

		//Renderer::GenerateMips(skyboxTextureCube);
	}
}