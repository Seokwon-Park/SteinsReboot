#pragma once

#include "Daydream.h"

namespace Daydream
{
	class EditorCamera
	{
	public:
		EditorCamera();

		void Update(Float32 _deltaTime);
		//void OnEvent(Event& e);

		void CameraRotate();
		void CameraMove(Float32 _deltaTime);

		void ControlCameraView(Float32 _deltaTime);

		inline const Matrix4x4& GetViewMatrix() const { return viewProjData.viewMatrix; }
		inline const Matrix4x4& GetProjectionMatrix() const { return viewProjData.projectionMatrix; };
		inline const Matrix4x4& GetViewProjectionMatrix() const { return viewProjData.viewProjectionMatrix; }
		const CameraData& GetCameraData() const;
		 
		Vector3 GetForward() const{return transform.GetForward();}
		Vector3 GetUp() const{return transform.GetUp();}
		Vector3 GetRight() const{return Vector3::Cross(GetUp(), GetForward());}

		inline void SetPosition(Vector3 _position);
		inline Vector3 GetPosition() const { return transform.position; }

		void UpdateAspectRatio(UInt32 _width, UInt32 _height);
		void SetProjectionType(ProjectionType _type) { projectionType = _type; }

		inline const Transform& GetTransform() const { return transform; }


	private:
		void UpdateViewProjectionMatrix();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

		Transform transform;

		ViewProjectionData viewProjData;

		Float32 nearPlane, farPlane;
		Float32 fovy;
		Float32 orthoSize;
		Float32 aspectRatio;

		ProjectionType projectionType = ProjectionType::Perspective;

		//Editor Var
		Vector2 mouseDelta = Vector2(0, 0);
		Vector2 prevMousePos = Vector2(0, 0);

		float rotationSpeed = 1.0f;
		float moveSpeed = 0.01f;
		bool isDragging = false;
	};
}