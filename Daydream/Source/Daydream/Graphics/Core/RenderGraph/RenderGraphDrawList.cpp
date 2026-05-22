#include "DaydreamPCH.h"
#include "RenderGraphDrawList.h"

namespace Daydream
{
	RenderGraphDrawList::RenderGraphDrawList()
	{
	}

	RenderGraphDrawList::~RenderGraphDrawList()
	{
	}
	void RenderGraphDrawList::AddDrawObject(Shared<Mesh> _mesh, Shared<Material> _material, const Matrix4x4& _transform, Float32 _distanceToCamera)
	{
		RenderGraphDrawObject item{};
		item.mesh = _mesh;
		item.material = _material;
		item.transform = _transform;
		item.distanceToCamera = _distanceToCamera;

		opaqueItems.push_back(item);
	}
}

