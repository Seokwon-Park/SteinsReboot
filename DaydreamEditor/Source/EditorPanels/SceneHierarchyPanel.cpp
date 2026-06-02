#include "SceneHierarchyPanel.h"

#include "imgui/imgui_internal.h"

namespace Daydream
{
	SceneHierarchyPanel::SceneHierarchyPanel()
	{
	}
	void SceneHierarchyPanel::SetCurrentContext(Shared<Scene> _scene)
	{
		scene = _scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		// 검색창
		ImGui::InputText("##Search", searchQuery, sizeof(searchQuery));
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Search for entities by name.");
		ImGui::Separator();

		if (scene)
		{
			// 루트 엔티티들만 순회 (재귀로 자식들 렌더링)
			auto& rootHandles = scene->GetRootEntities();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
				ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));

			for (UInt64 i = 0; i < rootHandles.size(); i++)
			{
				GameEntity* rootEntity = scene->GetEntity(rootHandles[i]);
				if (rootEntity)
				{
					// 엔티티 사이의 드롭 영역 (순서 변경용)
					DrawDropTargetBetween(EntityHandle(), i);

					DrawEntityNode(rootEntity);
				}
			}

			// 마지막 엔티티 아래의 드롭 영역
			DrawDropTargetBetween(EntityHandle(), rootHandles.size());

			ImGui::PopStyleVar();

			// 빈 공간 클릭 시 선택 해제
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
				!ImGui::IsAnyItemHovered() &&
				ImGui::IsWindowHovered())
			{
				selectedEntity = nullptr;
			}
		}

		// 우클릭 컨텍스트 메뉴
		if (ImGui::BeginPopupContextWindow("SceneHierarchyPopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				EntityHandle newHandle = scene->CreateGameEntity();
				if (GameEntity* entity = scene->GetEntity(newHandle))
				{
					entity->SetName("Empty Entity");
					selectedEntity = entity;
				}
			}
			if (ImGui::MenuItem("Create Camera"))
			{
				EntityHandle newHandle = scene->CreateGameEntity();
				if (GameEntity* entity = scene->GetEntity(newHandle))
				{
					entity->SetName("Camera");
					entity->AddComponent<CameraComponent>();
					selectedEntity = entity;
				}
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(GameEntity* _entity)
	{
		if (!_entity) return;

		// 검색 필터링
		String name = _entity->GetName();
		if (searchQuery[0] != '\0' && name.find(searchQuery) == String::npos)
		{
			return;
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_DrawLinesToNodes;

		if (selectedEntity == _entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		const auto& children = _entity->GetChildrenHandles();
		bool hasChildren = !children.empty();

		if (!hasChildren)
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool opened = ImGui::TreeNodeEx(
			(void*)(intptr_t)_entity->GetHandle().GetID(),
			flags,
			"%s",
			name.c_str()
		);

		if (ImGui::IsItemClicked())
		{
			selectedEntity = _entity;
		}

		// 드래그 소스 설정
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			DragDropPayload payload;
			payload.draggedEntityID = _entity->GetHandle().GetID();
			payload.isDragging = true;

			ImGui::SetDragDropPayload("SCENE_HIERARCHY_ENTITY", &payload, sizeof(DragDropPayload));
			ImGui::Text("Moving: %s", name.c_str());
			ImGui::EndDragDropSource();
		}

		// 드롭 타겟: 이 엔티티를 부모로 설정
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* imguiPayload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY"))
			{
				DragDropPayload* payload = (DragDropPayload*)imguiPayload->Data;
				EntityHandle draggedHandle(payload->draggedEntityID);
				GameEntity* draggedEntity = scene->GetEntity(draggedHandle);

				if (draggedEntity && draggedEntity != _entity)
				{
					draggedEntity->SetParent(_entity->GetHandle());
				}
			}
			ImGui::EndDragDropTarget();
		}

		// 우클릭 컨텍스트 메뉴
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Child Entity"))
			{
				EntityHandle newChild = scene->CreateGameEntity();
				if (GameEntity* entity = scene->GetEntity(newChild))
				{
					entity->SetName("Child Entity");
					entity->SetParent(_entity->GetHandle());
					selectedEntity = entity;
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate"))
			{
				// TODO: 엔티티 복제 기능
			}
			if (ImGui::MenuItem("Delete", "Del"))
			{
				//entityToDelete = entity;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Rename", "F2"))
			{
				// TODO: 이름 변경 기능
			}
			ImGui::EndPopup();
		}

		// 자식 엔티티들을 재귀적으로 렌더링
		if (opened)
		{
			if (hasChildren)
			{
				for (size_t i = 0; i < children.size(); ++i)
				{
					GameEntity* child = scene->GetEntity(children[i]);

					// 자식들 사이의 드롭 영역
					DrawDropTargetBetween(_entity->GetHandle(), i);

					if (child)
					{
						DrawEntityNode(child);
					}
				}

				// 마지막 자식 아래의 드롭 영역
				DrawDropTargetBetween(_entity->GetHandle(), children.size());

				ImGui::TreePop();
			}
		}
	}

	void SceneHierarchyPanel::DrawDropTargetBetween(EntityHandle _parentHandle, UInt64 _insertIndex)
	{
		// 1. 이 높이가 'TreeNode' 사이의 '전체' 간격이 됩니다.
			//    ImGui의 기본값인 4.0f 정도가 좋습니다. (너무 크면 간격이 넓어짐)
		const float dropZoneHeight = 2.0f;

		float windowWidth = ImGui::GetContentRegionAvail().x;
		if (windowWidth < 50.0f) windowWidth = 200.0f;

		// 2. InvisibleButton이 스스로 레이아웃 공간(4px)을 차지하게 둡니다.
		//    (커서 조작 로직(SetCursorPos)이 없는 것이 중요합니다)
		std::string buttonID = "##drop_zone_" + std::to_string(_parentHandle.GetID()) + "_" + std::to_string(_insertIndex);
		ImGui::InvisibleButton(buttonID.c_str(), ImVec2(windowWidth, dropZoneHeight));

		// 3. 버튼의 실제 영역 정보를 가져옵니다.
		ImVec2 rectMin = ImGui::GetItemRectMin();
		// ImVec2 rectMax = ImGui::GetItemRectMax(); // 라인만 그릴 거면 Max는 불필요

		// 4. 드롭 타겟 처리
		if (ImGui::BeginDragDropTarget())
		{
			// 5. ImGui 기본 노란색 사각형 끄기 (가려짐 문제 해결)
			if (const ImGuiPayload* imguiPayload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY"))
			{
				DragDropPayload* payload = (DragDropPayload*)imguiPayload->Data;
				EntityHandle draggedHandle(payload->draggedEntityID);
				GameEntity* draggedEntity = scene->GetEntity(draggedHandle);

				if (draggedEntity)
				{
					if (_parentHandle.IsValid())
					{
						GameEntity* parent = scene->GetEntity(_parentHandle);
						if (parent)
						{
							draggedEntity->SetParent(_parentHandle);
							// parent->ReorderChild(draggedHandle, _insertIndex);
						}
					}
					else
					{
						draggedEntity->SetParent(EntityHandle()); // 루트로 설정
						// scene->ReorderRootEntity(draggedHandle, _insertIndex);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}
	}

	void SceneHierarchyPanel::Update()
	{
		if (entityToDelete)
		{
			if (selectedEntity == entityToDelete)
			{
				selectedEntity = nullptr;
			}
			scene->DestroyEntity(entityToDelete->GetHandle());
			entityToDelete = nullptr;
		}

		// Delete 키 입력 처리
		if (selectedEntity && ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			entityToDelete = selectedEntity;
			selectedEntity = nullptr;
		}
	}
}
