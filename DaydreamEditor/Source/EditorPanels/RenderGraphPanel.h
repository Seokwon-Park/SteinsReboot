#pragma once

#include "Daydream.h"
#include "UIPanel.h"

#include "UI/Node/ImGuiNodeEditor.h"

namespace Daydream
{
	class RenderGraphPanel : public UIPanel
	{
	public:
		RenderGraphPanel();
		virtual ~RenderGraphPanel();

		void Setup(SceneRenderer* _sceneRenderer);

		virtual void OnImGuiRender() override;
	protected:

	private:
		SceneRenderer* sceneRenderer;

		Unique<ImGuiNodeEditor> nodeEditor;
		
	};
}
