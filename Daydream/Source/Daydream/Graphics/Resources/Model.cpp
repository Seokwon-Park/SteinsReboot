#include "DaydreamPCH.h"
#include "Model.h"

#include "Daydream/Graphics/Utility/ModelLoader.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Manager/RenderCacheManager.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	Model::Model(Shared<ModelData> _data)
	{
		rootNode = _data->rootNode;
	}
	Model::~Model()
	{
		meshes.clear();
		materials.clear();
	}
	void Model::Load(const Path & _path)
	{
		meshes.clear();

		//Path filePath(_path);

		//ModelData modelData = ModelLoader::LoadFromFile(filePath);

		//Vector3 vmin(1000, 1000, 1000);
		//Vector3 vmax(-1000, -1000, -1000);
		//for (auto meshData : modelData.meshes)
		//{
		//	// Normalize vertices

		//	for (auto& v : meshData.vertices) {
		//		vmin.x = std::min(vmin.x, v.position.x);
		//		vmin.y = std::min(vmin.y, v.position.y);
		//		vmin.z = std::min(vmin.z, v.position.z);
		//		vmax.x = std::max(vmax.x, v.position.x);
		//		vmax.y = std::max(vmax.y, v.position.y);
		//		vmax.z = std::max(vmax.z, v.position.z);
		//	}
		//}

		//for (auto meshData : modelData.meshes)
		//{
		//	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
		//	float dl = std::max(std::max(dx, dy), dz);
		//	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
		//		cz = (vmax.z + vmin.z) * 0.5f;

		//	for (auto& v : meshData.vertices)
		//	{
		//		v.position.x = (v.position.x - cx) / dl;
		//		v.position.y = (v.position.y - cy) / dl;
		//		v.position.z = (v.position.z - cz) / dl;
		//	}
		//	Shared<Mesh> mesh = Mesh::Create(meshData);
		//	meshes.push_back(mesh);
		//}

		//for (UInt32 i = 0; i < modelData.materials.size(); i++)
		//{
		//	//Shared<Material> newMat = Material::Create(ResourceManager::GetResource<PipelineState>("ForwardPSO"));
		//	Shared<Material> newMat = Material::Create(ResourceManager::GetResource<PipelineState>("GBufferPSO"));

		//	Shared<Texture2D> diffuse = AssetManager::GetAssetByPath<Texture2D>("DefaultTexture");
		//	Shared<Texture2D> normal = AssetManager::GetAssetByPath<Texture2D>("DefaultNormal");
		//	Shared<Texture2D> roughness = AssetManager::GetAssetByPath<Texture2D>("DefaultRoughness");
		//	Shared<Texture2D> metallic = AssetManager::GetAssetByPath<Texture2D>("DefaultMetallic");
		//	Shared<Texture2D> ao = AssetManager::GetAssetByPath<Texture2D>("DefaultAO");
		//	newMat->SetTexture2D("AlbedoTexture", diffuse);
		//	newMat->SetTexture2D("NormalTexture", normal);
		//	newMat->SetTexture2D("RoughnessTexture", roughness);
		//	newMat->SetTexture2D("MetallicTexture", metallic);
		//	newMat->SetTexture2D("AOTexture", ao);
		//	if (!modelData.materials[i].albedoMapPath.empty())
		//		newMat->SetTexture2D("AlbedoTexture", AssetManager::GetAssetByPath<Texture2D>(modelData.materials[i].albedoMapPath));
		//	if (!modelData.materials[i].normalMapPath.empty())
		//		newMat->SetTexture2D("NormalTexture", AssetManager::GetAssetByPath<Texture2D>(modelData.materials[i].normalMapPath));
		//	if (!modelData.materials[i].roughnessMapPath.empty())
		//		newMat->SetTexture2D("RoughnessTexture", AssetManager::GetAssetByPath<Texture2D>(modelData.materials[i].roughnessMapPath));
		//	if (!modelData.materials[i].metallicMapPath.empty())
		//		newMat->SetTexture2D("MetallicTexture", AssetManager::GetAssetByPath<Texture2D>(modelData.materials[i].metallicMapPath));
		//	if (!modelData.materials[i].AOMapPath.empty())
		//		newMat->SetTexture2D("AOTexture", AssetManager::GetAssetByPath<Texture2D>(modelData.materials[i].AOMapPath));

		//	materials.push_back(newMat);
		//}
	}

	Shared<Model> Model::Create(Shared<ModelData> _data)
	{
		return MakeShared<Model>(_data);
	}

	//Shared<Model> Model::Create(Shared<Mesh> _mesh)
	//{
	//	return MakeShared<Model>(_mesh);
	//}
}
