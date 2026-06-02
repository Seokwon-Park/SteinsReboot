#include "DaydreamPCH.h"
#include "Camera.h"

#include "Daydream/Graphics/Core/Renderer.h"


namespace Daydream
{
	Camera::Camera()
	{
		fovy = 60.0f;
		aspectRatio = 1.6f / 0.9f;
		orthoSize = 5.0f;
		nearPlane = 0.001f;
		farPlane = 1000.0f;

		viewProjectionBuffer = ConstantBuffer::Create(sizeof(CameraConstantBufferData));

		UpdateProjectionMatrix();
	}
	Camera::~Camera()
	{
	}

	void Camera::Update(Float32 _deltaTime)
	{

	}



}