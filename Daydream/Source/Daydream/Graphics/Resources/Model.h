#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Daydream/Asset/Asset.h"


namespace Daydream
{
	struct NodeData
	{
		String name;
		Transform transform = Transform();
		Int32 meshIndex = -1; // 이 노드가 메쉬를 보유하고 있는지?(modelData.meshes의 인덱스)
		Array<NodeData> children; // 자식 노드들
	};

	struct ModelData
	{
		Array<MeshData> meshes;
		Array<SkeletalMeshData> skeletalMeshes;
		Array<MaterialData> materials;

		NodeData rootNode;

		Bool isSkeletalModel = false;
	};

	class Model : public Asset
	{
	public:
		ASSET_CLASS_TYPE(Model)
		Model() = default;
		Model(Shared<ModelData> _data);
		//Model(Shared<Mesh> _mesh);
		~Model();
		
		void Load(const Path & _path);
		void AddMesh(AssetHandle _meshHandle) { meshes.push_back(_meshHandle); }
		void AddMaterial(AssetHandle _materialHandle) { materials.push_back(_materialHandle); }

		const Array<AssetHandle>& GetMeshes() const { return meshes; }
		const Array<AssetHandle>& GetMaterials() const { return materials; }
		const ModelData* GetModelData() const { return modelData.get(); }
		static Shared<Model> Create(Shared<ModelData> _data);
		//static Shared<Model> Create(Shared<Mesh> _mesh);
	private:
		Shared<ModelData> modelData;
		Array<AssetHandle> meshes;
		Array<AssetHandle> materials;
	};
}