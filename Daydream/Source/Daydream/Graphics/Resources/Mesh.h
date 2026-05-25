#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Material.h"



namespace Daydream
{
	static constexpr int maxBoneInfluence = 4;

	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector2 texCoord;
	};

	struct SkeletalVertex
	{
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector2 texCoord;

		Int32 boneIDs[maxBoneInfluence];
		Float32 weights[maxBoneInfluence];

		SkeletalVertex()
		{
			for (int i = 0; i < maxBoneInfluence; i++)
			{
				boneIDs[i] = -1;
				weights[i] = 0.0f;
			}
		}
	};

	struct MeshData
	{
		Array<Vertex> vertices;
		Array<UInt32> indices;
		Vector3 centerOffset;
		UInt32 materialIndex;
		Path basePath;
		String name;
	};

	struct SkeletalMeshData
	{
		Array<Vertex> vertices;
		Array<UInt32> indices;
		MaterialData materialData;
		Path basePath;
		String name;
		// TODO : 필요한것들
	};

	class Mesh : public Asset
	{
	public:
		ASSET_CLASS_TYPE(Mesh)
		Mesh() {};
		Mesh(const MeshData& _meshData);
		Mesh(Shared<VertexBuffer> _vertexBuffer, Shared<IndexBuffer> _indexBuffer);
		~Mesh();

		VertexBuffer* GetVertexBuffer() const { return vertexBuffer.get(); }
		IndexBuffer* GetIndexBuffer() const { return indexBuffer.get(); }
		UInt32 GetIndexCount() const { return indexBuffer->GetIndexCount(); }

		void Bind();

		static Shared<Mesh> Create();
		static Shared<Mesh> Create(const MeshData& _meshData);
		static Shared<Mesh> Create(Shared<VertexBuffer> _vertexBuffer, Shared<IndexBuffer> _indexBuffer);
	private:
		Shared<VertexBuffer> vertexBuffer;
		Shared<IndexBuffer> indexBuffer;
	};
}