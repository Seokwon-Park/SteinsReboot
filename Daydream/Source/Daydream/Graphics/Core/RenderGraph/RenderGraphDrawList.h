#pragma once

#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Material.h"

namespace Daydream
{
	struct RenderItem
	{
		Mesh* mesh;
		Material* material;
		Matrix4x4 worldMatrix;
		float distanceToCamera; // 카메라로부터의 거리
	};

	class RenderGraphDrawList
	{
	public:
		RenderGraphDrawList();
		~RenderGraphDrawList();

		void AddDrawObject(Mesh* _mesh, Material* _material, const Matrix4x4& _transform, Float32 _distanceToCamera);

		inline auto begin() { return renderItems.begin(); }
		inline auto end() { return renderItems.end(); }

		inline auto begin() const { return renderItems.begin(); }
		inline auto end() const { return renderItems.end(); }
	protected:

	private:
		Array<RenderItem> renderItems;
	};
}
