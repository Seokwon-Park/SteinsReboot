#pragma once

namespace Daydream
{
	struct ViewProjectionData
	{
		Matrix4x4 viewMatrix = Matrix4x4();
		Matrix4x4 projectionMatrix = Matrix4x4();
		Matrix4x4 viewProjectionMatrix = Matrix4x4();
	};
}
