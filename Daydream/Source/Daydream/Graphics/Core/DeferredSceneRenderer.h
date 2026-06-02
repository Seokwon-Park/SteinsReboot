#pragma once

#include "SceneRenderer.h"
#include "Daydream/Graphics/Resources/Struct/ViewProjectionData.h"

namespace Daydream
{
	class DeferredSceneRenderer : public SceneRenderer
	{
	public:
		DeferredSceneRenderer();
		virtual ~DeferredSceneRenderer();

		virtual void RenderScene(const SceneData& _sceneData) override;
	protected:

	private:
		ViewProjectionData lightViewProjData;

		Shared<ConstantBuffer> lightViewProjectionBuffer;
	};
}
