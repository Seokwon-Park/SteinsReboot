#include "DaydreamPCH.h"
#include "Transform.h"

namespace Daydream
{
	Transform::Transform()
	{
	}
	Transform::Transform(const Vector3& _position)
	{
		position = _position;
	}
	Transform::Transform(const Vector3& _position, const Vector3& _rotation)
	{
	}
	Transform::Transform(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale)
	{
	}

	//Vector3 Transform::GetForward()
	//{
	//	return glm::rotate(Quaternion(glm::radians(rotation)), Vector3(0.0f,0.0f,1.0f));
	//}
	//Vector3 Transform::GetUp()
	//{
	//	return glm::rotate(Quaternion(glm::radians(rotation)), Vector3(0.0f, 1.0f, 0.0f));
	//}
	//Vector3 Transform::GetRight()
	//{
	//	return glm::cross(GetUp(), GetForward());
	//}

	Vector3 Transform::GetForward() const
	{
		return Quaternion::RotateVector(Quaternion(Math::DegreeToRadian(rotation)), Vector3(0.0f, 0.0f, 1.0f));
	}

	Vector3 Transform::GetUp() const
	{
		return Quaternion::RotateVector(Quaternion(Math::DegreeToRadian(rotation)), Vector3(0.0f, 1.0f, 0.0f));
	}

	Vector3 Transform::GetRight() const
	{
		return Vector3::Cross(GetUp(), GetForward());
	}

	Quaternion Transform::GetOrientation()
	{
		return Quaternion(rotation);
	}

	Matrix4x4 Transform::CreateLocalMatrix(Transform _transform)
	{
		Matrix4x4 matT = Matrix4x4::CreateTranslation(_transform.position);
		Matrix4x4 matR = Matrix4x4::CreateRotation(Quaternion(Math::DegreeToRadian(_transform.rotation)));
		Matrix4x4 matS = Matrix4x4::CreateScale(_transform.scale);

		Matrix4x4 out;
		out = matS * matR * matT;

		return out;
	}

	Matrix4x4 Transform::GetLocalMatrix() const
	{
		Matrix4x4 localMatrix = CreateLocalMatrix(*this);
		return localMatrix;
	}

	bool Transform::Decompose(const Matrix4x4& _worldMat, Vector3& _outPosition, Vector3& _outRotation, Vector3& _outScale)
	{
		if (Math::IsNearlyEqual(_worldMat[3][3], 0.0f))
		{
			return false;
		}

		_outPosition = Vector3(_worldMat[3][0], _worldMat[3][1], _worldMat[3][2]);

		Vector3 xaxis(_worldMat[0][0], _worldMat[0][1], _worldMat[0][2]);
		Vector3 yaxis(_worldMat[1][0], _worldMat[1][1], _worldMat[1][2]);
		Vector3 zaxis(_worldMat[2][0], _worldMat[2][1], _worldMat[2][2]);

		_outScale.x = xaxis.Length();
		_outScale.y = yaxis.Length();
		_outScale.z = zaxis.Length();

		if (_outScale.x < Math::Epsilon ||
			_outScale.y < Math::Epsilon ||
			_outScale.z < Math::Epsilon)
		{
			return false;
		}

		xaxis /= _outScale.x;
		yaxis /= _outScale.y;
		zaxis /= _outScale.z;

		Quaternion rotation = Quaternion::CreateFromAxis(xaxis, yaxis, zaxis);
		_outRotation = rotation.ToEuler();
		_outRotation = Math::RadianToDegree(_outRotation);

		return true;
	}

	Transform Transform::Decompose(const Matrix4x4& _matrix)
	{
		Transform transform;
		if (Decompose(_matrix, transform.position, transform.rotation, transform.scale))
		{
			return transform;
		}
		else
		{
			return Transform();
		}
	}
}
