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
	void RenderGraphDrawList::AddDrawObject(Mesh* _mesh, Material* _material, const Matrix4x4& _transform, Float32 _distanceToCamera)
	{
		RenderItem item{};
		item.mesh = _mesh;
		item.material = _material;
		item.worldMatrix = _transform;
		item.distanceToCamera = _distanceToCamera;

		renderItems.push_back(item);
	}
	void RenderGraphDrawList::AddDrawObject(Mesh* _mesh)
	{
		RenderItem item{};
		item.mesh = _mesh;

		renderItems.push_back(item);
	}
}

