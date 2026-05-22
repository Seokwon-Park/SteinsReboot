#pragma once

#include "Daydream.h"
#include "UIPanel.h"


namespace Daydream
{
	class SceneHierarchyPanel : public UIPanel
	{
	public:
		SceneHierarchyPanel();

		void SetCurrentContext(Shared<Scene> _scene);
		void SetSelectedEntity(GameEntity* _entity) { selectedEntity = _entity; }
		GameEntity* GetSelectedEntity() { return selectedEntity; }

		void Update();
		void OnImGuiRender();
	private:
		struct DragDropPayload
		{
			UInt32 draggedEntityID;
			bool isDragging;
		};

		void DrawEntityNode(GameEntity * _entity);
		void DrawDropTargetBetween(EntityHandle _parentHandle, UInt64 _insertIndex);

		GameEntity* selectedEntity = nullptr;
		GameEntity* entityToDelete = nullptr;
		Shared<Scene> scene = nullptr;
		char searchQuery[256] = "";

		bool isDropTargetEnabled = false;
	};
}