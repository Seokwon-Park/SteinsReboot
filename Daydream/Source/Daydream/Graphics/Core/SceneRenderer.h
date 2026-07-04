#pragma once

#include "RenderGraph/RenderGraph.h"
#include "Daydream/Scene/Scene.h"
#include "Daydream/Graphics/Camera/Camera.h"
#include "Daydream/Graphics/Resources//Struct/RenderDataStruct.h"

namespace Daydream
{
    struct CameraData
    {
        Transform transform;
        ViewProjectionData viewProj;
    };

    struct SceneData
    {
        Scene* scene;
        CameraData cameraData;
        UInt32 width;
        UInt32 height;
    };

    struct SceneLightingData
    {
        DirectionalLight dirLights[2] = {};
        PointLight pointLights[4] = {};
        SpotLight spotLights[4] = {}; // 최대 32개 라이트
        Vector3 eyePos; 
        UInt32 dirLightCount = 0;
        Vector2 padding1;
        UInt32 pointLightCount = 0;
        UInt32 spotLightCount = 0;
    };

    class SceneRenderer
    {
    public:
        SceneRenderer();
        virtual ~SceneRenderer();

        virtual void RenderScene(const SceneData& _sceneData) {};

        Texture2D* GetResult() const { return result.texture.get(); };
        TextureView* GetResultView() const { return result.texture->GetDefaultSRV(); };

        Texture2D* GetShadowMap() const { return shadowMap.texture.get(); };
        TextureView* GetShadowMapView() const { return shadowMap.texture->GetDefaultSRV(); };

        inline RenderGraph* GetRenderGraph() const { return renderGraph.get(); }
    protected:
        RenderGraphDrawList CreateDrawListFromScene(Scene* _scene, const CameraData& _cameraData);
        void PrepareLighting(Scene* _scene, const CameraData& _cameraData);
        void ValidateResultTexture(UInt32 width, UInt32 height);

        LightComponent* GetLightComponent() const { return mainLightComponent; }

        LightComponent* mainLightComponent = nullptr;

        Texture2DAllocation result;
        Texture2DAllocation shadowMap;

        SceneLightingData lightData;
        ViewProjectionData lightViewProj;
        ViewProjectionData cameraViewProj;
        Shared<RenderGraph> renderGraph;

        
    };
}
