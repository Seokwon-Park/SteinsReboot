#pragma once

#include "Daydream.h"
#include "UIPanel.h"

#include "imgui_node_editor.h"

namespace ImNode = ax::NodeEditor;

namespace Daydream
{
	struct LinkInfo
	{
		ImNode::LinkId Id;
		ImNode::PinId  InputId;
		ImNode::PinId  OutputId;
	};

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

		ImNode::EditorContext* context = nullptr;
		bool                 isFirstFrame = true;    // Flag set for first frame only, some action need to be executed once.
		ImVector<LinkInfo>   links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		int                  nextLinkId = 100;
	};
}
