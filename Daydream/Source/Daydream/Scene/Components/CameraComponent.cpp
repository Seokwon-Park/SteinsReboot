#include "DaydreamPCH.h"
#include "CameraComponent.h"

#include "Daydream/Scene/GameEntity/GameEntity.h"
#include "Daydream/Scene/Components/TransformComponent.h"


namespace Daydream
{
	CameraComponent::CameraComponent()
	{
		fovy = 60.0f;
		aspectRatio = 1.6f / 0.9f;
		orthoSize = 5.0f;
		nearPlane = 0.001f;
		farPlane = 1000.0f;

		UpdateProjectionMatrix();
	}
	CameraComponent::~CameraComponent()
	{
	}
	void CameraComponent::Init()
	{
	}
	void CameraComponent::Update(Float32 _deltaTime)
	{
	}

	void CameraComponent::UpdateViewProjectionMatrix()
	{
		viewProjData.viewProjectionMatrix = viewProjData.viewMatrix * viewProjData.projectionMatrix;
		viewProjData.viewProjectionMatrix.Transpose();
	}
	void CameraComponent::UpdateViewMatrix()
	{
		TransformComponent* transform = GetOwner()->GetComponent<TransformComponent>();

		if (transform)
		{
			viewProjData.viewMatrix = transform->GetWorldMatrix().Inversed();
			UpdateViewProjectionMatrix();
		}
	}

	void CameraComponent::UpdateProjectionMatrix()
	{
		switch (projectionType)
		{
		case ProjectionType::Perspective:
			viewProjData.projectionMatrix = Matrix4x4::CreatePerspectiveLH(Math::DegreeToRadian(fovy), aspectRatio, nearPlane, farPlane);
			break;
		case ProjectionType::Orthographic:
		{
			viewProjData.projectionMatrix = Matrix4x4::CreateOrthographicLH(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, nearPlane, farPlane);
			break;
		}
		default:
			break;
		}
		UpdateViewProjectionMatrix();
	}
}
