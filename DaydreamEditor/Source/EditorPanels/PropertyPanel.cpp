#include "PropertyPanel.h"

#include "Daydream/Scene/Components/ComponentRegistry.h"

namespace Daydream
{
	PropertyPanel::PropertyPanel()
	{
	}

	void PropertyPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");
		
		if (selectedEntity)
		{
            char buffer[256];
            strncpy(buffer, selectedEntity->GetName().c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = 0;

            ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;

            if (ImGui::BeginTable("##EntityNameTable", 2, flags)) // 고유 ID, 열 2개, 플래그
            {
                // ImGuiTableColumnFlags_WidthFixed: 초기 너비 고정
                // ImGuiTableColumnFlags_WidthStretch: 창 크기에 따라 너비 조절 (기본값)
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Name"); // 왼쪽 열: 레이블

                ImGui::TableSetColumnIndex(1);
                // [컨트롤러 UI 코드] // 오른쪽 열: 컨트롤러
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2,0 });

                ImGui::SameLine();
                if (ImGui::InputText("##Name", buffer, 256))
                {
                    selectedEntity->SetName(buffer);
                };

                ImGui::PopStyleVar();
                ImGui::EndTable();
            }
            ImGui::Separator(); // 이름 입력창 아래에 구분선을 추가
            
            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_Framed
                | ImGuiTreeNodeFlags_SpanAvailWidth
				| ImGuiTreeNodeFlags_AllowOverlap;

            auto& components = selectedEntity->GetAllComponents();
            for (size_t i = 0; i < components.size(); ++i)
            {
                Component* component = components[i].get();
                const String& name = component->GetName();

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
                flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

                bool opened = ImGui::TreeNodeEx((void*)component, flags, "%s", name.c_str());
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("PROPERTY_ITEM", &i, sizeof(size_t));

                    ImGui::Text("%s", name.c_str());

                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROPERTY_ITEM"))
                    {
                        size_t draggedItemIndex = *(const size_t*)payload->Data;

                        if (draggedItemIndex != i)
                        {
                            selectedEntity->SwapComponentOrder(draggedItemIndex ,i);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if (opened)
                {
                    //draw component Editor
                    DrawComponentProperties(component);
                    ImGui::TreePop();
                }
            }
            
            ImGui::Spacing(); // 버튼 위에 약간의 공간 추가
            ImGui::Separator(); // 구분선

            const char* buttonLabel = "Add Component";
            float buttonWidth = ImGui::CalcTextSize(buttonLabel).x + ImGui::GetStyle().FramePadding.x * 2.0f;
            float contentWidth = ImGui::GetContentRegionAvail().x;
            float cursorPosX = (contentWidth - buttonWidth) * 0.5f;
            if (cursorPosX > 0.0f)
            {
                ImGui::SetCursorPosX(cursorPosX);
            }

            if (ImGui::Button(buttonLabel))
            {
                ImGui::OpenPopup("AddComponentPopup");
            }

            if (ImGui::BeginPopup("AddComponentPopup"))
            {
                // 등록된 모든 컴포넌트 목록을 가져옴
                const auto& componentFactoryMap = ComponentRegistry::GetComponentMap();

                // 검색 기능 (선택 사항이지만 매우 유용)
                static char searchQuery[256] = "";
                ImGui::InputTextWithHint("##Search", "Search Component...", searchQuery, sizeof(searchQuery));
                ImGui::Separator();

                for (const auto& [name, componentFunctions] : componentFactoryMap)
                {
                    String searchQueryLower = searchQuery;
                    std::transform(searchQueryLower.begin(), searchQueryLower.end(), searchQueryLower.begin(), tolower);

                    // 2. 비교 대상인 컴포넌트 이름(name)도 소문자로 변환
                    String nameLower = name;
                    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), tolower);

                    // 검색어 필터링
                    if (searchQuery[0] != '\0' && nameLower.find(searchQueryLower) == std::string::npos)
                    {
                        continue; // 검색어와 일치하지 않으면 건너뜀
                    }

                    if (componentFunctions.hasFunc(selectedEntity))
                    {
                        continue;
                    }

                    // 목록에서 컴포넌트 이름을 클릭하면
                    if (ImGui::Selectable(name.c_str()))
                    {
                        componentFunctions.createFunc(selectedEntity);

                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }
		}

		ImGui::End();
	}
    void PropertyPanel::DrawComponentProperties(Component* _component)
    {
        const auto& fields = _component->GetFields();

        for (const auto& field : fields)
        {
            switch (field.type)
            {
            case FieldType::Float:
            {
                float* data = static_cast<float*>(field.data);
                ImGui::DragFloat(field.name.c_str(), data, 0.1f);
                break;
            }
            case FieldType::Vector3:
            {
                float* data = static_cast<float*>(field.data);
                ImGui::DragFloat3(field.name.c_str(), data, 0.1f);
                break;
            }
            case FieldType::Transform:
            {
                Transform* data = static_cast<Transform*>(field.data);
                if (UI::DrawTransformController("Transform", *data))
                {
                    _component->OnPropertyChanged();
                }
                break;
            }
            case FieldType::Texture:
            {
                ImGui::Text("%s", field.name.c_str());
                ImGui::SameLine();
                break;
            }
            case FieldType::Light:
            {
                Light* data = static_cast<Light*>(field.data);
                UI::DrawLightController("Light", *data);
                break;
            }
            case FieldType::ModelPtr:
            {
                Model* data = static_cast<Model*>(field.data);
                UI::DrawModelController("Model", data);
                break;
            }

            case FieldType::Mesh:
            {
                AssetHandle* handle = static_cast<AssetHandle*>(field.data);
                UI::DrawMeshController("Mesh", handle);
                break;
            }
            case FieldType::Material:
            {
                AssetHandle* handle = static_cast<AssetHandle*>(field.data);
                UI::DrawMaterialController("Material", handle);
                break;
            }
            case FieldType::PBRValue:
            {
                MaterialConstantBufferData* data = static_cast<MaterialConstantBufferData*>(field.data);
                UI::DrawPBRController("MaterialOptions", *data);
                break;
            }
            }
        }
    }
}


