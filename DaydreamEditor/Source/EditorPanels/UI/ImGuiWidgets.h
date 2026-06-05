#pragma once

#include "Daydream/Graphics/Resources/Light.h"
#include "Daydream/Graphics/Resources/Model.h"

namespace Daydream::UI
{
	bool DrawFloatController(const String& _label, Float32& _value, Float32 _speed = 0.1f, Float32 _minValue = 0.0f, Float32 _maxValue = 0.0f, Float32 _resetValue = 0.0f, Float32 _columnWidth = 100.0f);

	bool DrawAxisControl(const String& _label, Float32& _value, Float32 _speed = 0.1f, Float32 _minValue = 0.0f, Float32 _maxValue = 0.0f, Float32 _resetValue = 0.0f, Vector4 _color = { 0.2f, 0.2f,0.2f,1.0f });
	bool DrawVector3Controller(const String&_label, Vector3& _values,
		Float32 _resetValue = 0.0f, Float32 _columnWidth = 100.0f);
	bool DrawTransformController(const String& _label, Transform& _transform,
		Float32 _resetValue = 0.0f, Float32 _columnWidth = 100.0f);

	void DrawColorController(const String& _label, Vector3& _color, Float32 _resetValue = 0.0f, Float32 _columnWidth = 100.0f);
	//void MaterialControl(const String& _material);

	void DrawMaterialController(const String& _label, Material* _material);

	void DrawLightController(const String& _label, Light& _light);
	void DrawModelController(const String& _label, Model* _model);
	
	void DrawMeshController(const String& _label, AssetHandle* _meshHandle);
	void DrawMaterialController(const String& _label, AssetHandle* _materialHandle);


	void DrawPBRController(const String& _label, MaterialConstantBufferData& _data);
	bool CheckboxU32(const char* label, UInt32* v);

	bool DrawToggleButtonGroup(const char* _label, int* _selectedIndex, const Array<String>& _options);
}