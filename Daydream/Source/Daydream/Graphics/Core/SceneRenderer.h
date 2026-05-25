#pragma once

#include "RenderGraph/RenderGraph.h"
#include "Daydream/Scene/Scene.h"
#include "Daydream/Graphics/Camera/Camera.h"

namespace Daydream
{
    struct SceneData
    {
        Scene* scene;
        Camera* camera;
        UInt32 width;
        UInt32 height;
    };

    class SceneRenderer
    {
    public:
        SceneRenderer();
        virtual ~SceneRenderer();

        virtual void RenderScene(const SceneData& _sceneData) {};
    protected:
        RenderGraphDrawList CreateDrawListFromScene(Scene* _scene, Camera* _camera);
       
        Shared<RenderGraph> renderGraph;
    };
}
