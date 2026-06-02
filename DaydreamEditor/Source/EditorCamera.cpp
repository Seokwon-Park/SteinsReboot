#include "DaydreamPCH.h"
#include "EditorCamera.h"

#include "Daydream/Core/Input.h"

namespace Daydream
{
	EditorCamera::EditorCamera()
	{
		fovy = 60.0f;
		aspectRatio = 1.6f / 0.9f;
		orthoSize = 5.0f;
		nearPlane = 0.001f;
		farPlane = 1000.0f;

		UpdateProjectionMatrix();
	}

	void EditorCamera::Update(Float32 _deltaTime)
	{
		const Vector2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		mouseDelta = (mouse - prevMousePos);


		prevMousePos = mouse;
	}

	void EditorCamera::CameraRotate()
	{
		transform.rotation.y += mouseDelta.x * rotationSpeed;
		transform.rotation.x += mouseDelta.y * rotationSpeed;
	}

	void EditorCamera::CameraMove(Float32 _deltaTime)
	{
		if (Input::GetKeyPressed(Key::Q))
		{
			SetPosition(transform.position - transform.GetUp() * _deltaTime);
		}
		if (Input::GetKeyPressed(Key::E))
		{
			SetPosition(transform.position + transform.GetUp() * _deltaTime);
		}

		if (Input::GetKeyPressed(Key::W))
		{
			SetPosition(transform.position + transform.GetForward() * _deltaTime);
		}

		if (Input::GetKeyPressed(Key::A))
		{
			SetPosition(transform.position - transform.GetRight() * _deltaTime);
		}

		if (Input::GetKeyPressed(Key::S))
		{
			SetPosition(transform.position - transform.GetForward() * _deltaTime);
		}

		if (Input::GetKeyPressed(Key::D))
		{
			SetPosition(transform.position + transform.GetRight() * _deltaTime);
		}
	}
	void EditorCamera::ControlCameraView(Float32 _deltaTime)
	{
		CameraRotate();
		CameraMove(_deltaTime);

		UpdateViewMatrix();
	}
	const CameraData& EditorCamera::GetCameraData() const
	{
		CameraData data;
		data.transform = transform;
		data.viewProj = viewProjData; 
		return data;
	}

	inline void EditorCamera::SetPosition(Vector3 _position)
	{
		transform.position = _position;
	}

	void EditorCamera::UpdateAspectRatio(UInt32 _width, UInt32 _height)
	{
		aspectRatio = static_cast<Float32>(_width) / _height;
		UpdateProjectionMatrix();
	}


	void EditorCamera::UpdateViewProjectionMatrix()
	{
		viewProjData.viewProjectionMatrix = viewProjData.viewMatrix * viewProjData.projectionMatrix;
	}
	void EditorCamera::UpdateViewMatrix()
	{
		viewProjData.viewMatrix = Matrix4x4::CreateLookToLH(transform.position, transform.GetForward(), transform.GetUp());
		UpdateViewProjectionMatrix();
	}

	void EditorCamera::UpdateProjectionMatrix()
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
