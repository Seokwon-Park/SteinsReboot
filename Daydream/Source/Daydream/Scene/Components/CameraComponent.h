#pragma once

#include "Component.h"
#include "Daydream/Graphics/Resources/Struct/ViewProjectionData.h"

namespace Daydream
{

	class CameraComponent : public Component
	{
	public:
		DEFINE_COMPONENT_NAME(CameraComponent);

		CameraComponent();
		virtual ~CameraComponent();

		virtual void Init() override;
		virtual void Update(Float32 _deltaTime) override;

		virtual void OnPropertyChanged() override{};

		const ViewProjectionData& GetCameraViewProj() { return viewProjData; }


		REFLECT_START()
		REFLECT_END()
	private:
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateViewProjectionMatrix();

		Float32 nearPlane, farPlane;
		Float32 fovy;
		Float32 orthoSize;
		Float32 aspectRatio;

		ProjectionType projectionType = ProjectionType::Perspective;

		ViewProjectionData viewProjData;
	};
}