#include "SteinsPCH.h"
#include "Camera.h"

namespace Steins
{
	Camera::Camera()
		:viewMatrix(Matrix4x4())
	{
		direction = Vector3(0.0f, 0.0f, 1.0f);
		up = Vector3(0.0f, 1.0f, 0.0f);
		fovy = 70.0f;
		UpdateMatrix();
	}
	Camera::~Camera()
	{
	}
	const Matrix4x4& Camera::GetViewMatrix() const
	{
		return viewMatrix;
	}
	const Matrix4x4& Camera::GetProjectionMatrix() const
	{
		return projectionMatrix;
	}
	const Matrix4x4& Camera::GetViewProjectionMatrix() const
	{
		return viewProjectionMatrix;
	}
	void Camera::SetPosition(Vector3 _position)
	{
		position = _position;
		UpdateMatrix();
	}
	void Camera::UpdateMatrix()
	{
		viewMatrix = Matrix4x4::LookTo(position, direction, up);
		//viewMatrix = Matrix4x4::Inverse(Matrix4x4::Translate({ -position.x, -position.y, -position.z }));
		switch (projectionType)
		{
		case ProjectionType::Perspective:
			projectionMatrix = Matrix4x4::Perspective(fovy, 1.6f, 0.1f, 10.0f);
			break;
		case ProjectionType::Orthographic:
		{
			projectionMatrix = Matrix4x4::Orthographic(-1.6f, 1.6f, -0.9f, 0.9f, 0.1f, 100.0f);
			break;
		}
		default:
			break;
		}
		viewProjectionMatrix = projectionMatrix * viewMatrix;
		viewProjectionMatrix.MatrixTranspose();
	}
}