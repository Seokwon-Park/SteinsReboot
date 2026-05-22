#pragma once

#include <Daydream.h>
#include "EditorPanels/SceneHierarchyPanel.h"
#include "EditorPanels/ViewportPanel.h"
#include "EditorPanels/PropertyPanel.h"
#include "EditorPanels/AssetBrowserPanel.h"
#include "EditorPanels/SkyboxPanel.h"


namespace Daydream
{
	struct EntityInfo
	{
		UInt32 dummy[2]{};
		UInt32 entityID;
		Int32 thickness;
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Float32 _deltaTime) override;
		virtual void OnImGuiRender() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& _event) override;

		bool OnKeyPressed(KeyPressedEvent& _e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& _e);

		void CreateProject();

		void BeginDockspace();

		void UpdateViewportSize();

		Pair<Int32, Int32> GetViewportMousePos();
	private:
		Vector2 mainWindowSize;
		Vector2 viewportSize;
		Vector2 viewportBounds[2];

		bool isViewportHovered = false;
		bool isViewportFocused = false;

		bool isGuizmoInteract = false;
		bool viewportShouldResize = true;

		bool isViewControlled = false;

		Shared<SceneRenderer> sceneRenderer;

		Shared<VertexBuffer> cubeVBO;
		Shared<IndexBuffer> cubeIBO;

		Shared<Sampler> sampler;
		Shared<Texture2D> texture;

		Shared<ConstantBuffer> viewProjMat;
		EntityInfo info;
		Shared<ConstantBuffer> entityBuffer;

		Shared<GraphicsPipelineState> depthPSO;
		Shared<GraphicsPipelineState> gBufferPSO;
		Shared<GraphicsPipelineState> deferredLightingPSO;
		Shared<GraphicsPipelineState> pso;
		Shared<GraphicsPipelineState> pso3d;
		Shared<GraphicsPipelineState> skyboxPipeline;
		Shared<GraphicsPipelineState> equirectangleToCubePipeline;
		Shared<GraphicsPipelineState> maskPSO;

		Shared<Mesh> cubeMesh;
		Shared<Model> model;
				
		Shared<Scene> activeScene;

		Shared<EditorCamera> editorCamera;
		Shared<Project> currentProject;

		Unique<AssetManager> editorAssetManager;

		UInt32 viewIndex = 0;

		//Panels
		Array<UIPanel*> editorPanels;

		Unique<ViewportPanel> viewportPanel;
		Unique<PropertyPanel> propertyPanel;
		Unique<SceneHierarchyPanel> sceneHierarchyPanel;
		Unique<AssetBrowserPanel> assetBrowserPanel;
		Unique<SkyboxPanel> skyboxPanel;

		UInt32 guizmoType = -1;
	};
}
