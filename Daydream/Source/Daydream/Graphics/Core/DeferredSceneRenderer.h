#pragma once

#include "SceneRenderer.h"

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

	};
}
