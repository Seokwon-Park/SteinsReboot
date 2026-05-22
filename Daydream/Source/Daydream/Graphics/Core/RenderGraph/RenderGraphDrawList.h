#pragma once

#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Material.h"

namespace Daydream
{
	struct RenderGraphDrawObject
	{
		Shared<Mesh> mesh;
		Shared<Material> material;
		Matrix4x4 transform;
		float distanceToCamera; // 카메라로부터의 거리
	};

	class RenderGraphDrawList
	{
	public:
		RenderGraphDrawList();
		~RenderGraphDrawList();

		void AddDrawObject(Shared<Mesh> _mesh, Shared<Material> _material, const Matrix4x4& _transform, Float32 _distanceToCamera);
	protected:

	private:
		Array<RenderGraphDrawObject> opaqueItems;
	};
}
