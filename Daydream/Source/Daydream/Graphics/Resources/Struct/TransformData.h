#pragma once

namespace Daydream
{
	struct TransformConstantBufferData
	{
		Matrix4x4 world;
		Matrix4x4 worldInverseTranspose;
	};
}
