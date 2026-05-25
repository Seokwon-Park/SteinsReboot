#include "DaydreamPCH.h"
#include "ImGuiWidgets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Texture/TextureCube.h"
#include "Daydream/Graphics/Resources/Framebuffer.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Utility/MeshGenerator.h"
#include "Daydream/Asset/AssetManager.h"

namespace Daydream::UI
{
	void DrawFloatController(const String& _label, Float32& _value, Float32 _speed, Float32 _minValue, Float32 _maxValue, Float32 _resetValue, Float32 _columnWidth)
	{
		ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;

		if (ImGui::BeginTable(_label.c_str(), 2, flags)) // 고유 ID, 열 2개, 플래그
		{
			// 열의 속성을 설정합니다. (너비, 크기 조절 등)
			// ImGuiTableColumnFlags_WidthFixed: 초기 너비 고정
			// ImGuiTableColumnFlags_WidthStretch: 창 크기에 따라 너비 조절 (기본값)
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(_label.c_str()); // 왼쪽 열: 레이블

			ImGui::TableSetColumnIndex(1);
			// [컨트롤러 UI 코드] // 오른쪽 열: 컨트롤러
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2,0 });
			float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
			Vector2 buttonSize = { lineHeight + 2.0f , lineHeight };

			ImGui::SameLine();
			std::string dragID = "##" + _label;
			ImGui::DragFloat(dragID.c_str(), &_value, _speed, _minValue, _maxValue, "%.2f");

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}
	}

	void DrawAxisControl(const String& _label, Float32& _value, Float32 _speed, Float32 _minValue, Float32 _maxValue, Float32 _resetValue, Vector4 _color)
	{
		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec4 color(_color.x, _color.y, _color.z, _color.w);
		ImVec2 buttonSize = { lineHeight + 2.0f , lineHeight };
		// 각 버튼의 색상 설정
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ color.x - 0.1f, color.y - 0.1f, color.z - 0.1f, 1.0f });

		// 굵은 폰트 적용
		if (ImGui::Button(_label.c_str(), buttonSize))
		{
			_value = _resetValue;
		}
		ImGui::PopStyleColor(3);

		// 드래그 슬라이더
		ImGui::SameLine();
		std::string dragID = "##" + _label;
		ImGui::DragFloat(dragID.c_str(), &_value, _speed, _minValue, _maxValue, "%.2f");
	}

	void DrawVector3Controller(const String& _label, Vector3& _values, Float32 _resetValue, Float32 _columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;

		ImGui::PushID(_label.c_str());
		if (ImGui::BeginTable(_label.c_str(), 2, flags)) // 고유 ID, 열 2개, 플래그
		{
			// 열의 속성을 설정합니다. (너비, 크기 조절 등)
			// ImGuiTableColumnFlags_WidthFixed: 초기 너비 고정
			// ImGuiTableColumnFlags_WidthStretch: 창 크기에 따라 너비 조절 (기본값)
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(_label.c_str()); // 왼쪽 열: 레이블

			ImGui::TableSetColumnIndex(1);
			// [컨트롤러 UI 코드] // 오른쪽 열: 컨트롤러
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2,0 });
			ImGui::PushFont(boldFont);

			DrawAxisControl("X", _values.x, 0.1f, 0.0f, 0.0f, 0.0f, { 0.8f, 0.1f,0.1f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::SameLine();
			DrawAxisControl("Y", _values.y, 0.1f, 0.0f, 0.0f, 0.0f, { 0.1f, 0.8f,0.1f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::SameLine();
			DrawAxisControl("Z", _values.z, 0.1f, 0.0f, 0.0f, 0.0f, { 0.1f, 0.1f,0.8f, 1.0f });

			ImGui::PopItemWidth();

			ImGui::PopFont();
			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::Columns(1);

		ImGui::PopID();
	}
	void DrawVector3Controller(const String& _label, Vector3& _values, const char* _Xtag, const char* _Ytag, const char* _Ztag, Float32 _resetValue, Float32 _columnWidth)
	{

	}
	void DrawTransformController(const String& _label, Transform& _transform, Float32 _resetValue, Float32 _columnWidth)
	{
		DrawVector3Controller("Position", _transform.position);
		DrawVector3Controller("Rotation", _transform.rotation);
		DrawVector3Controller("Scale", _transform.scale);
	}
	void DrawMaterialController(const String& _label, Material* _material)
	{
		ImGui::Text(_label.c_str());
		ImGui::Separator();

		for (auto& [name, textureBinding] : _material->GetTextureBindings())
		{
			ImGui::Text(name.c_str());
			if (textureBinding.cache != nullptr)
				ImGui::Image(textureBinding.cache->GetImGuiHandle(), ImVec2{ 100,100 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetManager::AssetTypeToString(AssetType::Texture2D).c_str()))
				{
					AssetHandle* pHandle = (AssetHandle*)payload->Data;
					AssetHandle handle = *pHandle;
					// auto texture = AssetManager::GetAsset<Texture2D>(handle);
					_material->SetTextureBinding(name, handle);
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
	void DrawLightController(const String& _label, Light& _light)
	{
		ImGui::PushID(_label.c_str());
		const char* lightTypeNames[] = { "Directional", "Point", "Spot" };

		int currentItem = _light.type;

		if (ImGui::Combo("##Light Type", &currentItem, lightTypeNames, IM_ARRAYSIZE(lightTypeNames)))
		{
			// 사용자가 드롭다운 메뉴에서 다른 항목을 선택했다면,
			// 변경된 인덱스 값을 다시 enum 타입으로 변환하여 원본 데이터에 저장합니다.
			_light.type = currentItem;
		}

		switch (_light.type)
		{
		case Directional:
			DrawFloatController("Intensity", _light.intensity, 0.1f, 0.0f, 10000.0f);
			DrawColorController("Color", _light.color);
			break;
		case Point:
			DrawFloatController("Intensity", _light.intensity, 0.1f, 0.0f, 10000.0f);
			DrawFloatController("Range", _light.range, 0.1f, 0.0f, 0.0f);
			DrawColorController("Color", _light.color);
			break;
		case Spot:
			DrawFloatController("Intensity", _light.intensity, 0.1f, 0.0f, 10000.0f);
			DrawFloatController("Range", _light.range, 0.1f, 0.0f, 0.0f);
			DrawFloatController("InnerConeAngle", _light.spotInnerAngle, 0.1f, 0.0f, _light.spotOuterAngle);
			DrawFloatController("OuterConeAngle", _light.spotOuterAngle, 0.1f, 0.0f, 360.0f);
			DrawColorController("Color", _light.color);
			break;
		default:
			break;
		}


		/*DrawColorEditor("Color", _light.color);*/

		ImGui::PopID();
	}
	void DrawModelController(const String& _label, Model* _model)
	{
		ImGui::PushID(_label.c_str());
		//const auto& modelNames = ResourceManager::GetInstance().
		//const char* currentItemName = (modelRef) ? modelRef->GetName().c_str() : "None";

		//if (ImGui::BeginCombo(label, currentItemName))
		//{
		//	if (ImGui::Selectable("None", modelRef == nullptr))
		//	{
		//		modelRef = nullptr;
		//	}
		//	// ... (나머지 로직은 거의 동일) ...

		//	for (const auto& modelName : modelNames)
		//	{
		//		bool isSelected = (modelRef && modelRef->GetName() == modelName);
		//		if (ImGui::Selectable(modelName.c_str(), isSelected))
		//		{
		//			// ResourceManager에서 바로 Shared<Model>을 받아 할당
		//			modelRef = ResourceManager::GetResource<Model>(modelName);
		//		}
		//		// ...
		//	}
		//	ImGui::EndCombo();
		//}

		//auto& materials = _model->GetMaterials();
		//for (UInt64 i = 0; i < materials.size(); i++)
		//{
		//	String label = "Material" + std::to_string(i);
		//	DrawMaterialController(label, materials[i].get());
		//}

		ImGui::PopID();
	}

	void DrawMeshController(const String& _label, AssetHandle* _meshHandle)
	{
		AssetMetadata metadata = AssetManager::GetAssetMetadata(*_meshHandle);
		const char* previewValue = metadata.name.empty() ? "None" : metadata.name.c_str();

		// 3. 콤보 박스 시작
		// 첫 번째 인자: 라벨(ID), 두 번째 인자: 닫혀있을 때 보이는 텍스트
		if (ImGui::BeginCombo(_label.c_str(), previewValue))
		{
			// 4. 목록 순회
			for (auto& metadata : AssetManager::GetAssetsByType(AssetType::Mesh))
			{
				// 이 아이템이 선택된 상태인지 확인
				// (포인터 주소 비교 혹은 UUID 비교)
				const bool isSelected = (*_meshHandle == metadata.handle);

				// 5. 선택 가능한 항목 그리기 (Selectable)
				// 클릭 시 true 반환 -> 선택된 것으로 처리
				if (ImGui::Selectable(metadata.name.c_str(), isSelected))
				{
					*_meshHandle = metadata.handle;
				}

				// 6. 초기 포커스 설정 (선택된 아이템으로 바로 스크롤 이동)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		//ImGui::PopID();
	}

	void DrawMaterialController(const String& _label, AssetHandle* _materialHandle)
	{
		AssetMetadata metadata = AssetManager::GetAssetMetadata(*_materialHandle);
		const char* previewValue = metadata.name.empty() ? "None" : metadata.name.c_str();

		// 3. 콤보 박스 시작
		// 첫 번째 인자: 라벨(ID), 두 번째 인자: 닫혀있을 때 보이는 텍스트
		if (ImGui::BeginCombo(_label.c_str(), previewValue))
		{
			// 4. 목록 순회
			for (auto& metadata : AssetManager::GetAssetsByType(AssetType::Material))
			{
				// 이 아이템이 선택된 상태인지 확인
				// (포인터 주소 비교 혹은 UUID 비교)
				const bool isSelected = (*_materialHandle == metadata.handle);

				// 5. 선택 가능한 항목 그리기 (Selectable)
				// 클릭 시 true 반환 -> 선택된 것으로 처리
				//Shared<Material> material = AssetManager::GetAsset<Material>(metadata.handle);
				if (ImGui::Selectable(metadata.name.c_str(), isSelected))
				{
					*_materialHandle = metadata.handle;
				}

				// 6. 초기 포커스 설정 (선택된 아이템으로 바로 스크롤 이동)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	bool CheckboxU32(const char* label, UInt32* v)
	{
		bool b = (*v != 0); // uint를 bool로 변환
		if (ImGui::Checkbox(label, &b))
		{
			*v = b ? 1 : 0; // 변경된 bool을 다시 uint로 변환
			return true;
		}
		return false;
	}

	void DrawPBRController(const String& _label, MaterialConstantBufferData& _data)
	{
		ImGui::PushID(_label.c_str());
		CheckboxU32("use AlbedoMap", &_data.useAlbedoMap);
		if (!_data.useAlbedoMap)
		{
			DrawColorController("Albedo", _data.albedo, 1.0f);
		}
		CheckboxU32("use NormalMap", &_data.useNormalMap);
		CheckboxU32("use AOMap", &_data.useAOMap);
		CheckboxU32("use MetallicMap", &_data.useMetallicMap);
		if (!_data.useMetallicMap)
		{
			DrawFloatController("MetallicFactor", _data.metallic, 0.005f, 0.0f, 1.0f);
		}
		CheckboxU32("use RoughnessMap", &_data.useRoughnessMap);
		if (!_data.useRoughnessMap)
		{
			DrawFloatController("RoughnessFactor", _data.roughness, 0.005f, 0.0f, 1.0f);
		}
		DrawFloatController("Exposure", _data.exposure, 0.005f, 0.0f, 1.0f);
		DrawFloatController("Gamma", _data.gamma, 0.005f, 0.0f, 10.0f);


		ImGui::PopID();
	}

	void DrawColorController(const String& _label, Vector3& _color, Float32 _resetValue, Float32 _columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;
		ImGuiColorEditFlags colorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_Float;

		if (ImGui::BeginTable(_label.c_str(), 2, flags))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(_label.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2,0 });
			ImGui::PushFont(boldFont);
			float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
			Vector2 buttonSize = { lineHeight + 2.0f , lineHeight };

			DrawAxisControl("R", _color.x, 0.01f, 0.0f, 1.0f, 0.0f, { 0.8f, 0.1f,0.1f, 1.0f });
			ImGui::PopItemWidth();
			ImGui::SameLine();
			DrawAxisControl("G", _color.y, 0.01f, 0.0f, 1.0f, 0.0f, { 0.1f, 0.8f,0.1f, 1.0f });
			ImGui::PopItemWidth();
			ImGui::SameLine();
			DrawAxisControl("B", _color.z, 0.01f, 0.0f, 1.0f, 0.0f, { 0.1f, 0.1f,0.8f, 1.0f });
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// 1. 임시 변수 선언 및 데이터 변환 (0~255  ->  0~1)
			ImGui::ColorEdit3("##Color", &_color.x, colorEditFlags);

			ImGui::PopFont();
			ImGui::PopStyleVar();

			ImGui::EndTable();
		}
	}

	bool DrawToggleButtonGroup(const char* _label, int* _selectedIndex, const Array<String>& _options)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		bool valueChanged = false;

		ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;
		ImGuiColorEditFlags colorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_Float;
		if (ImGui::BeginTable(_label, 2, flags))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(_label);

			ImGui::TableSetColumnIndex(1);

			// [추가] 버튼 사이의 가로 간격을 0으로 설정
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			// 스타일 설정 (이전과 동일)
			ImVec4 selectedColor = ImVec4(0.98f, 0.7f, 0.02f, 1.0f);
			ImVec4 unselectedColor = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
			ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);

			// [추가] 남은 가로 공간을 버튼 개수만큼 균등하게 계산
			// options.size()가 0인 경우를 대비해 max(1, ...) 처리
			float buttonWidth = ImGui::GetContentRegionAvail().x / std::max(1, (Int32)_options.size());

			for (int i = 0; i < _options.size(); ++i)
			{
				// 첫 번째 버튼이 아닌 경우에만 SameLine을 호출 (ItemSpacing이 0이므로 필요 없어 보이지만,
				// 명시적으로 줄 바꿈을 막기 위해 유지하는 것이 안전합니다.)
				if (i > 0) {
					ImGui::SameLine();
				}

				bool isSelected = (*_selectedIndex == i);

				if (isSelected) {
					ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, unselectedColor);
				}

				// [수정] ImGui::Button의 두 번째 인자로 계산된 너비를 전달 (높이는 0으로 두어 자동으로 설정)
				if (ImGui::Button(_options[i].c_str(), ImVec2(buttonWidth, 0)))
				{
					*_selectedIndex = i;
					valueChanged = true;
				}

				ImGui::PopStyleColor(1);
			}

			ImGui::PopStyleColor(1);
			ImGui::PopStyleVar(2); // ItemSpacing과 FrameBorderSize 총 2개를 Pop

			ImGui::EndTable();

		}
		return valueChanged;
	}
}
