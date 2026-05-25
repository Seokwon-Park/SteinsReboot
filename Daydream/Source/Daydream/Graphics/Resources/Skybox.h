#pragma once

#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureCube.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Core/RenderingInfo.h"

namespace Daydream
{
    class Skybox
    {
    public:
        Skybox();
        ~Skybox();

        void CreateResources();
        void GenerateDefault();
        void Update();

        void GenerateHDRCubemap(Texture2D* _texture);
        void GenerateIrradianceCubemap();
        void GeneratePrefilterCubemap();
        void GenerateBRDF();
        void UpdateSkyboxFace(UInt32 _faceIndex, Shared<Texture2D> _texture);

        inline void SetSkyboxResolution(UInt32 _value) { skyboxResolution = _value; }
        inline void SetDiffuseResolution(UInt32 _value) { diffuseResolution = _value; }
        inline void SetSpecularResolution(UInt32 _value) { specularResolution = _value; }
        inline UInt32 GetPrefilterMipLevel() const { return (UInt32)std::log2(specularResolution); }

        inline Shared<TextureView> GetSkyboxFaceSRV(UInt32 _faceIndex)
        {
            DAYDREAM_ASSERT(_faceIndex < 6, "{} < 6", _faceIndex);
            return skyboxFaceSRVs[_faceIndex];
        }

        inline Shared<TextureView> GetIrradianceFaceSRV(UInt32 _faceIndex)
        {
            DAYDREAM_ASSERT(_faceIndex < 6, "{} < 6", _faceIndex);
            return irradianceSRVs[_faceIndex];
        }

        inline Shared<TextureView> GetPrefilterFaceSRV(UInt32 _faceIndex, UInt32 _mipLevel)
        {
            UInt32 index = prefilterMipLevels * _faceIndex + _mipLevel;
            DAYDREAM_ASSERT(index < prefilterMipLevels * 6, "{} < {}", index, prefilterMipLevels * 6);

            return prefilterSRVs[index];
        }

        inline Shared<TextureView> GetBRDFSRV()
        {
            return BRDFSRV;
        }

        inline Shared<Texture2D> GetBRDFTexture() 
        {
            return BRDFTexture; 
        }
        inline Shared<TextureCube> GetSkyboxTexture()
        {
            return skyboxTextureCube; 
        }
        inline Shared<TextureCube> GetIrradianceTexture()
        {
            return irradianceTextureCube; 
        }
        inline Shared<TextureCube> GetPrefilterTexture()
        { 
            return prefilterTextureCube; 
        }
    private:

        UInt32 skyboxMipLevels = 0;
        UInt32 prefilterMipLevels = 0;

        Bool isUsingSkybox = true;
        Bool isHDR = false;

        UInt32 skyboxResolution = 512;
        UInt32 diffuseResolution = 64;
        UInt32 specularResolution = 512;

        Vector4 roughness = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

        Matrix4x4 cubeFaceProjMatrix;
        Array<Matrix4x4> cubeFaceViewMatrices;
        Array<Matrix4x4> captureViewProjections;
        Array<Shared<ConstantBuffer>> cubeFaceConstantBuffers;
        Array<Shared<ConstantBuffer>> roughnessConstantBuffers;

        //Shared<Material> equirectangularMaterial;
        //Array<Shared<Material>> equirectangularMaterials;
        //Array<Shared<Material>> irradianceMaterials;
        //Array<Shared<Material>> prefilterMaterials;
        //Shared<Material> resizeMaterial;

        //Shared<Framebuffer> captureFramebuffer;
        //Shared<Framebuffer> resizeFramebuffer;
        //Shared<Framebuffer> irradianceFramebuffer;
        //Array<Shared<Framebuffer>> prefilterFramebuffers;

        //Shared<RenderPass> equirectangularRenderPass;
        //Shared<RenderPass> resizeRenderPass;
        //Shared<RenderPass> irradianceRenderPass;

        GraphicsPipelineState* equirectangularPSO;
        GraphicsPipelineState* irradiancePSO;
        GraphicsPipelineState* prefilterPSO;
        GraphicsPipelineState* resizePSO;
        GraphicsPipelineState* brdfPSO;

        Mesh* boxMesh;
        Mesh* quadMesh;
        
        Texture2D* equirectangularTexture; // 2D->Cube 원본 텍스쳐
        Texture2D* equirectangularDropTarget; // dummy(No Texture)
        Shared<TextureView> equirectangularSRV; // 2D->Cube 원본 텍스쳐뷰

        Array<Shared<Texture2D>> equirectangularResultTextures;
        Array<Shared<Texture2D>> irradianceResultTextures;
        Array<Shared<Texture2D>> prefilterResultTextures;

        Shared<TextureCube> skyboxTextureCube;
        Array<Shared<TextureView>> skyboxFaceRTVs;
        Array<Shared<TextureView>> skyboxFaceSRVs;

        Shared<TextureCube> irradianceTextureCube;
        Shared<TextureView> irradianceDSV;
        Array<Shared<TextureView>> irradianceRTVs;
        Array<Shared<TextureView>> irradianceSRVs;

        Shared<TextureCube> prefilterTextureCube;
        Array<Shared<TextureView>> prefilterRTVs;
        Array<Shared<TextureView>> prefilterSRVs;
        
        Shared<Texture2D> BRDFTexture;
        Shared<TextureView> BRDFRTV;
        Shared<TextureView> BRDFSRV;

        Shared<Texture2D> resizeTexture;
        Shared<TextureView> resizeRTV;
    };
}