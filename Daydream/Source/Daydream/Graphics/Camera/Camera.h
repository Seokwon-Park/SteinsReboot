#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"

namespace Daydream
{
	struct CameraConstantBufferData
	{
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 viewProjection;
	};

	class Camera
	{
	public:
		Camera();
		~Camera();

		virtual void Update(Float32 _deltaTime);


		Shared<ConstantBuffer> GetViewProjectionConstantBuffer() const { return viewProjectionBuffer; }

		void SetProjectionType(ProjectionType _type) { projectionType = _type; }

	protected:
		void UpdateViewProjectionMatrix();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

		Shared<ConstantBuffer> viewProjectionBuffer;

		Float32 nearPlane, farPlane;
		Float32 fovy;
		Float32 orthoSize;
		Float32 aspectRatio;

		ProjectionType projectionType = ProjectionType::Perspective;
	};
}