#include "DaydreamPCH.h"
#include "ModelLoader.h"

namespace Daydream
{
	namespace
	{
		Matrix4x4 ConvertAssimpMatrix(const aiMatrix4x4& _mat)
		{
			// 엔진의 행렬 메모리 레이아웃(Row/Column Major)에 맞춰 변환
			Matrix4x4 ret(&_mat.a1);

			return ret;
		}

		bool CheckIsSkeletal(const aiScene* _scene)
		{
			// 1. 애니메이션이 있으면 무조건 스켈레탈로 간주
			if (_scene->mNumAnimations > 0) return true;

			// 2. 메쉬들을 뒤져서 뼈가 있는지 확인
			for (unsigned int i = 0; i < _scene->mNumMeshes; i++)
			{
				aiMesh* mesh = _scene->mMeshes[i];
				if (mesh->HasBones())
				{
					return true;
				}
			}

			return false;
		}
	}

	Shared<ModelData> ModelLoader::LoadFromFile(const Path& _filePath)
	{
		Assimp::Importer importer;

		Path baseDirectory;
		baseDirectory = _filePath.GetParentPath();

		UInt32 flags = aiProcess_Triangulate |   // 모든 면을 삼각형으로 변환
			//aiProcess_PreTransformVertices |
			aiProcess_ConvertToLeftHanded; // | // 왼손 좌표계 사용

		//aiProcess_OptimizeGraph;// |
		//aiProcess_FlipUVs |				// UV 좌표 뒤집기 (OpenGL용)
		//aiProcess_GenNormals |			// 노말 벡터 생성
	//	aiProcess_CalcTangentSpace;			// | //탄젠트/바이탄젠트 계산
	//aiProcess_JoinIdenticalVertices | // 중복 정점 제거
	//aiProcess_OptimizeMeshes |		// 메시 최적화
	//aiProcess_ValidateDataStructure;  // 데이터 유효성 검사

		DAYDREAM_INFO("Load File {}", _filePath.ToGenericString());

		const aiScene* scene = importer.ReadFile(_filePath.ToString(), flags);
		bool result = scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode;
		DAYDREAM_CORE_ASSERT(!result && scene != nullptr, "{0}", importer.GetErrorString());

		Shared<ModelData> modelData;

		modelData = MakeShared<ModelData>();
		modelData->meshes.clear();
		modelData->materials.clear();

		if (scene->mNumMaterials > 0)
			modelData->materials.reserve(scene->mNumMaterials);
		if (scene->mNumMeshes > 0)
			modelData->meshes.reserve(scene->mNumMeshes);

		for (UInt32 i = 0; i < scene->mNumMeshes; i++)
		{
			ProcessMesh(scene->mMeshes[i], scene, modelData);
		}

		for (UInt32 i = 0; i < scene->mNumMaterials; i++)
		{
			ProcessMaterial(scene->mMaterials[i], modelData, baseDirectory);
		}

		//Vector3 vmin(1000, 1000, 1000);
		//Vector3 vmax(-1000, -1000, -1000);
		//for (auto meshData : modelData->meshes)
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

		//for (auto& meshData : modelData->meshes)
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
		//}

		Matrix4x4 identity;
		ProcessNode(scene->mRootNode, scene, modelData->meshes, modelData->rootNode);
		return modelData;
	}

	void ModelLoader::ProcessScene(const aiScene* _scene)
	{
		//ProcessNode(_scene->mRootNode, _scene, );
	}
	void ModelLoader::ProcessNode(aiNode* _curNode, const aiScene* _scene, const Array<MeshData>& _meshDatas, NodeData& _curNodeData)
	{
		// 현재 노드의 Transform의 worldMatrix를 가져온다.
		Matrix4x4 localTransformMatrix = ConvertAssimpMatrix(_curNode->mTransformation);

		bool hasMesh = (_curNode->mNumMeshes > 0);

		// 만약 메쉬도 없고 matrix도 identity이고 자식노드도 1개라면 이 노드는 굳이 필요가 없다.
		if (!hasMesh && localTransformMatrix.IsIdentity() && _curNode->mNumChildren == 1)
		{
			ProcessNode(_curNode->mChildren[0], _scene, _meshDatas, _curNodeData);
			return;
		}
		
		_curNodeData.name = _curNode->mName.C_Str();
		_curNodeData.transform = Transform::Decompose(localTransformMatrix);

		UInt32 meshSize = _scene->mNumMeshes;
		//이 노드에 메쉬가 하나인 경우 이 노드가 메쉬를 가진다.
		if (_curNode->mNumMeshes == 1)
		{
			UInt32 meshIndex = _curNode->mMeshes[0];
			if (meshIndex < meshSize)
			{
				_curNodeData.meshIndex = meshIndex;
				_curNodeData.materialIndex = _scene->mMeshes[meshIndex]->mMaterialIndex;
				//Matrix4x4 tmp = Matrix4x4::CreateTranslation(_meshDatas[meshIndex].centerOffset) * nextTransform;
				//node.transform = Transform::Decompose(tmp);

			}
			//translation += rotation * (centerOffset * scale);
		}
		//메쉬가 여러개인 경우 자식을 메쉬 수만큼 만들어준다.
		else if (_curNode->mNumMeshes > 1)
		{
			for (UInt32 i = 0; i < _curNode->mNumMeshes; i++)
			{
				UInt32 meshIndex = _curNode->mMeshes[i];

				// 서브 노드 생성
				NodeData subNodeData{};
				subNodeData.name = _curNodeData.name + "_Mesh_" + std::to_string(i);
				subNodeData.meshIndex = meshIndex;
				subNodeData.materialIndex = _scene->mMeshes[meshIndex]->mMaterialIndex;
				
				//SubNode의 경우 curNodeData의 Transform을 기준으로 0,0,0이 되므로 transform을 설정할 필요가 없음

				// 자식으로 등록
				_curNodeData.children.push_back(subNodeData);
			}
		}

		// 자식 노드 순회(위에서 생성한 자식 메쉬 노드들은 NodeData, 여기는 aiNode*의 child)
		for (UInt32 i = 0; i < _curNode->mNumChildren; i++)
		{
			NodeData childNode;
			ProcessNode(_curNode->mChildren[i], _scene, _meshDatas, childNode);
			_curNodeData.children.push_back(childNode);
		}

	}
	void ModelLoader::ProcessMesh(aiMesh* _mesh, const aiScene* _scene, Shared<ModelData> _modelData)
	{
		MeshData meshData;
		meshData.name = _mesh->mName.C_Str();

		// AABB 계산을 위한 변수 초기화
		Vector3 minBound(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 maxBound(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (UInt32 i = 0; i < _mesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.position.x = _mesh->mVertices[i].x;
			vertex.position.y = _mesh->mVertices[i].y;
			vertex.position.z = _mesh->mVertices[i].z;

			minBound = Math::Min(minBound, vertex.position);
			maxBound = Math::Max(maxBound, vertex.position);

			if (_mesh->HasNormals())
			{
				vertex.normal.x = _mesh->mNormals[i].x;
				vertex.normal.y = _mesh->mNormals[i].y;
				vertex.normal.z = _mesh->mNormals[i].z;
			}

			if (_mesh->HasTangentsAndBitangents())
			{
				vertex.tangent.x = _mesh->mTangents[i].x;
				vertex.tangent.y = _mesh->mTangents[i].y;
				vertex.tangent.z = _mesh->mTangents[i].z;
			}

			if (_mesh->mTextureCoords[0])
			{
				vertex.texCoord.x = _mesh->mTextureCoords[0][i].x;
				vertex.texCoord.y = _mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoord = Vector2(0.0f, 0.0f);
			}

			meshData.vertices.push_back(vertex);
		}

		//DAYDREAM_CORE_TRACE("{},{},{}", minBound.x, minBound.y, minBound.z);
		//DAYDREAM_CORE_TRACE("{},{},{}", maxBound.x, maxBound.y, maxBound.z);
		//Vector3 center = (minBound + maxBound) * 0.5f;
		//meshData.centerOffset = center;
		////이 메쉬의 중심이 0,0,0에서 얼마나 떨어져 있나?

		////// 3. 정점 재배치 (recentering)
		////// 모든 정점에서 중심점을 빼서, 메쉬의 로컬 원점을 (0,0,0)인 중심으로 이동시킴
		//for (auto& v : meshData.vertices)
		//{
		//	v.position = v.position - center;
		//}

		for (UInt32 i = 0; i < _mesh->mNumFaces; i++)
		{
			aiFace face = _mesh->mFaces[i];
			for (UInt32 j = 0; j < face.mNumIndices; j++)
			{
				meshData.indices.push_back(face.mIndices[j]);
			}
		}
		
		_modelData->meshes.push_back(meshData);
	}
	void ModelLoader::ProcessMaterial(aiMaterial* _material, Shared<ModelData> _modelData, const Path& _baseDirectory)
	{
		MaterialData materialData;
		aiString name;
		_material->Get(AI_MATKEY_NAME, name);
		materialData.name = name.C_Str();

		materialData.albedoMapPath = StringGetTexturePath(_material, aiTextureType_DIFFUSE, _baseDirectory);
		//GetTexturePath(_material, aiTextureType_SPECULAR, materialData.specularTexturePath);
		materialData.normalMapPath = StringGetTexturePath(_material, aiTextureType_NORMALS, _baseDirectory);
		materialData.roughnessMapPath = StringGetTexturePath(_material, aiTextureType_DIFFUSE_ROUGHNESS, _baseDirectory);
		materialData.metallicMapPath = StringGetTexturePath(_material, aiTextureType_METALNESS, _baseDirectory);
		materialData.AOMapPath = StringGetTexturePath(_material, aiTextureType_AMBIENT_OCCLUSION, _baseDirectory);

		if (materialData.AOMapPath.empty())
		{
			if (!materialData.roughnessMapPath.empty() &&
				materialData.roughnessMapPath == materialData.metallicMapPath)
			{
				materialData.AOMapPath = materialData.roughnessMapPath;
			}
			else
			{
				materialData.AOMapPath = StringGetTexturePath(_material, aiTextureType_LIGHTMAP, _baseDirectory);
			}
		}
		aiColor4D diffuseColor;
		if (aiGetMaterialColor(_material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS)
		{
			materialData.diffuseColor = Vector3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
		}

		float shininess;
		if (aiGetMaterialFloat(_material, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS)
		{
			materialData.shininess = shininess;
		}
		_modelData->materials.push_back(materialData);
	}

	String ModelLoader::StringGetTexturePath(aiMaterial* _mat, aiTextureType _type, const Path& _baseDirectory)
	{
		// 해당 타입의 텍스처가 1개 이상 있는지 확인하고, 첫 번째 텍스처 경로를 가져옴
		if (_mat->GetTextureCount(_type) > 0)
		{
			aiString str;
			if (_mat->GetTexture(_type, 0, &str) == AI_SUCCESS)
			{
				return (_baseDirectory / str.C_Str()).ToGenericString();
			}
		}
		return "";
	}
}