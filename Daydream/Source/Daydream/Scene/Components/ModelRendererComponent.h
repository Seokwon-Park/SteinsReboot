#pragma once

#include "Component.h"
#include "Daydream/Graphics/Resources/Model.h"

namespace Daydream
{
	class ModelRendererComponent : public Component
	{
	public:
		DEFINE_COMPONENT_NAME(ModelRendererComponent);

		ModelRendererComponent();
		virtual ~ModelRendererComponent();

		virtual void Init() override;
		virtual void Update(Float32 _deltaTime) override {};

		void SetModel(Model* _model);

		REFLECT_START()
			ADD_PROPERTY(FieldType::PBRValue, materialValue)
			ADD_PTR_PROPERTY(FieldType::ModelPtr, model)
		REFLECT_END()
	private:
		Model* model;
		MaterialConstantBufferData materialValue;
		Shared<ConstantBuffer> worldMatrix;
		Shared<ConstantBuffer> materialCB;
		Shared<ConstantBuffer> entityHandle;

		Shared<Material> maskMaterial;

	};
}