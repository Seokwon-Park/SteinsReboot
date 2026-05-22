#include "DaydreamPCH.h"
#include "AssetManager.h"

#include "AssetImporter.h"
#include "AssetDefaults.h"

#include "Daydream/Graphics/Resources/Texture/Texture2D.h"

#include "Daydream/Core/UUID.h"
#include "yaml-cpp/yaml.h"

namespace Daydream
{
	namespace
	{
		LoadPhase GetAssetLoadPhase(AssetType type)
		{
			switch (type)
			{
				// [Early] 의존성이 없는 원시 데이터
			case AssetType::Texture2D:
			case AssetType::Shader:
				//case AssetType::Mesh:
				//case AssetType::Audio:
				return LoadPhase::Early;

				// [Normal] 원시 데이터를 참조하는 데이터
			case AssetType::Model:
			case AssetType::Material: // Texture, Shader 참조
				//case AssetType::Animation:
				//case AssetType::PhysicsMaterial:
				return LoadPhase::Normal;

				// [Late] 모든 것이 준비된 후 로드해야 하는 데이터
			//case AssetType::Prefab:   // Mesh, Material 등 모든걸 참조
			case AssetType::Scene:
				return LoadPhase::Late;
			}

			return LoadPhase::MAX; // 기본값
		}
	}

	const SortedMap<String, AssetType> AssetManager::assetExtensionMap = {
		// 텍스처
		{".png", AssetType::Texture2D},
		{".jpg", AssetType::Texture2D},
		{".jpeg", AssetType::Texture2D},
		{".tga", AssetType::Texture2D},
		{".hdr", AssetType::Texture2D},

		// 씬
		{".ddscene", AssetType::Scene}, // 예시

		// 셰이더
		{".hlsl", AssetType::Shader},

		// 메쉬
		{".fbx", AssetType::Model},
		{".FBX", AssetType::Model},
		{".gltf", AssetType::Model},
		{".obj", AssetType::Model},

		//머티리얼
		{".ddmat", AssetType::Material},


		// 스크립트
	};

	const SortedMap<String, AssetType> AssetManager::assetTypeMap =
	{
		{"Texture2D", AssetType::Texture2D},
		{"Model", AssetType::Model},
		{"Shader", AssetType::Shader},
		{"Mesh", AssetType::Mesh},
		{"Material", AssetType::Material},
	};


	String AssetManager::AssetTypeToString(AssetType _type)
	{
		switch (_type)
		{
		case AssetType::None:
			return "None";
			break;
		case AssetType::Texture2D:
			return "Texture2D";
			break;
		case AssetType::Scene:
			break;
		case AssetType::Shader:
			return "Shader";
			break;
		case AssetType::Model:
			return "Model";
			break;
		case AssetType::Mesh:
			return "Mesh";
			break;
		case AssetType::Material:
			return "Material";
			break;
		case AssetType::Script:
			break;
		default:
			break;
		}
		DAYDREAM_CORE_ERROR("case is not valid");
		return "";
	}

	Array<AssetMetadata> AssetManager::GetAssetsByType(AssetType type)
	{
		Array<AssetMetadata> result;
		for (const auto& [handle, metadata] : instance->assetRegistry)
		{
			if (metadata.type == type)
			{
				result.push_back(metadata);
			}
		}
		return result;
	}

	AssetManager::AssetManager()
	{

	}

	void AssetManager::Init()
	{
		if (instance == nullptr)
		{
			DAYDREAM_CORE_INFO("[AssetManager] AssetManager initialize");
			instance = new AssetManager();
		}
		else
		{
			DAYDREAM_CORE_WARN("[AssetManager] AssetManager is already initialized!");
		}
	}

	void AssetManager::Shutdown()
	{
		if (instance != nullptr)
		{
			DAYDREAM_CORE_INFO("[AssetManager] AssetManager shutdown");
			delete instance;
			instance = nullptr;
		}
	}

	void AssetManager::LoadAssetMetadataFromDirectory(const Path& _directoryPath, bool _isRecursive)
	{
		instance->ProcessDirectory(_directoryPath, _isRecursive);
	}

	void AssetManager::LoadAssets(LoadPhase _phase)
	{
		instance->CreateBuiltinTexture2D();
		for (auto [handle, metadata] : instance->assetRegistry)
		{
			if (instance->loadedAssetCache.find(handle) != instance->loadedAssetCache.end())
			{
				continue;
			}

			LoadPhase phase = GetAssetLoadPhase(metadata.type);
			//if (phase != _phase) continue;
		}

		//	Shared<Asset> newAsset = instance->LoadAssetCache(metadata.handle);
		//	newAsset->SetAssetHandle(metadata.handle);

		//	if (newAsset == nullptr)
		//	{
		//		return;
		//	}

		//	newAsset->SetAssetHandle(metadata.handle);
		//	instance->loadedAssetCache[metadata.handle] = newAsset;
		//}
	}

	void AssetManager::CreateBuiltinAssets()
	{
		instance->CreateBuiltinTexture2D();
		AssetMetadata metadata;
		metadata.handle = AssetDefaults::DefaultMaterial;
		metadata.filePath = "";
		metadata.type = AssetType::Material;
		metadata.name = "";

		instance->assetRegistry[AssetDefaults::DefaultMaterial] = metadata;
	}

	const AssetMetadata& AssetManager::GetAssetMetadata(AssetHandle _handle)
	{
		auto itr = instance->assetRegistry.find(_handle);
		if (itr == instance->assetRegistry.end())
		{
			return instance->assetRegistry[AssetDefaults::DefaultMaterial];
		}
		return itr->second;
	}

	AssetHandle AssetManager::GetAssetHandleByPath(const Path& _path)
	{
		auto itr = instance->assetPathMap.find(_path.ToGenericString());
		if (itr == instance->assetPathMap.end())
		{
			return AssetHandle();
		}
		return itr->second;
	}

	AssetType AssetManager::GetAssetTypeFromExtension(String _ext)
	{
		auto itr = assetExtensionMap.find(_ext);
		if (itr == assetExtensionMap.end())
		{
			return AssetType::None;
		}
		return itr->second;
	}

	AssetMetadata AssetManager::LoadMetadata(const Path& _metaFilePath)
	{
		YAML::Node metaNode = YAML::LoadFile(_metaFilePath.ToString());
		if (!metaNode["Handle"])
		{
			return AssetMetadata(); // 유효하지 않음
		}

		AssetMetadata metadata;
		metadata.handle = AssetHandle(metaNode["Handle"].as<String>());
		metadata.filePath = metaNode["Path"].as<String>();
		metadata.type = StringToAssetType(metaNode["Type"].as<String>());
		metadata.name = metaNode["Name"].as<String>();

		switch (metadata.type)
		{
		case AssetType::Texture2D:
			break;
		case AssetType::TextureCube:
			//loadedAsset = AssetImporter::LoadTextureCube(metadata.FilePath);
			break;
		case AssetType::Model:
		{
			for (auto subAsset : metaNode["SubAssets"])
			{
				String meshName = subAsset.first.as<String>();
				YAML::Node meshNode = subAsset.second;

				AssetMetadata subAssetMetadata;
				subAssetMetadata.handle = AssetHandle(meshNode["Handle"].as<String>());
				subAssetMetadata.filePath = meshNode["Path"].as<String>();
				subAssetMetadata.type = StringToAssetType(meshNode["Type"].as<String>());
				subAssetMetadata.name = meshNode["Name"].as<String>();

				metadata.subAssets[subAssetMetadata.filePath.ToGenericString()] = subAssetMetadata;
				DAYDREAM_CORE_INFO("{}", subAsset.first.as<String>());
			}
			break;
		}
		case AssetType::Shader:
			break;
		default:
			break;
			// 로드할 수 없는 타입
		}
		return metadata;
	}

	Shared<Asset> AssetManager::LoadAssetCache(AssetHandle _uuid)
	{
		// 1. 메타데이터 레지스트리에서 애셋 정보 찾기
		auto itr = assetRegistry.find(_uuid);
		if (itr == assetRegistry.end())
		{
			DAYDREAM_CORE_WARN("Cannot find AssetMetaData!");
			return nullptr;
		}

		const AssetMetadata& metadata = itr->second;

		// 2. 메타데이터의 'Type'에 따라 적절한 임포터 호출
		Shared<Asset> loadedAsset = nullptr;
		switch (metadata.type)
		{
		case AssetType::Texture2D:
			loadedAsset = AssetImporter::LoadTexture2D(metadata);
			break;
		case AssetType::TextureCube:
			//loadedAsset = AssetImporter::LoadTextureCube(metadata.FilePath);
			break;
		case AssetType::Model:
			DAYDREAM_CORE_INFO(metadata.handle.ToString());
			loadedAsset = AssetImporter::LoadModel(metadata);
			break;
			// ... 기타 애셋 타입
		case AssetType::Material:
			loadedAsset = AssetImporter::LoadMaterial(metadata);
			break;
		case AssetType::Shader:
			loadedAsset = AssetImporter::LoadShader(metadata);
			break;
		default:
			// 로드할 수 없는 타입
			return nullptr;
		}

		if (loadedAsset == nullptr)
		{
			// 파일이 없거나, 임포트 중 손상됨
			return nullptr;
		}

		// 3. 로드 성공 -> 캐시에 저장
		loadedAssetCache[_uuid] = loadedAsset;
		loadedAsset->SetAssetName(metadata.name);

		// 4. 로드된 애셋(Shared<Asset>) 반환
		return loadedAsset;
	}

	void AssetManager::CreateBuiltinTexture2D()
	{
		Texture2DDesc desc{};
		desc.width = 1;
		desc.height = 1;
		desc.format = RenderFormat::R8G8B8A8_UNORM;
		desc.textureUsage = TextureUsage::ShaderResource;
		UInt32 imageSize = desc.width * desc.height;

		Array<UInt8> pixelData;
		pixelData.resize(imageSize * 4);
		pixelData[0] = 255;
		pixelData[1] = 255;
		pixelData[2] = 255;
		pixelData[3] = 255;
		assetPathMap["DefaultTexture"] = AssetDefaults::DefaultAlbedoHandle;
		loadedAssetCache[AssetDefaults::DefaultAlbedoHandle] = Texture2D::Create(desc, pixelData.data());

		pixelData[0] = 128;
		pixelData[1] = 128;
		pixelData[2] = 255;
		assetPathMap["DefaultNormal"] = AssetDefaults::DefaultNormalHandle;
		loadedAssetCache[AssetDefaults::DefaultNormalHandle] = Texture2D::Create(desc, pixelData.data());

		pixelData[0] = 128;
		pixelData[1] = 128;
		pixelData[2] = 128;
		assetPathMap["DefaultRoughness"] = AssetDefaults::DefaultRoughnessHandle;
		loadedAssetCache[AssetDefaults::DefaultRoughnessHandle] = Texture2D::Create(desc, pixelData.data());

		pixelData[0] = 0;
		pixelData[1] = 0;
		pixelData[2] = 0;
		assetPathMap["DefaultMetallic"] = AssetDefaults::DefaultMetallicHandle;
		loadedAssetCache[AssetDefaults::DefaultMetallicHandle] = Texture2D::Create(desc, pixelData.data());

		pixelData[0] = 255;
		pixelData[1] = 255;
		pixelData[2] = 255;
		assetPathMap["DefaultAO"] = AssetDefaults::DefaultAOHandle;
		loadedAssetCache[AssetDefaults::DefaultAOHandle] = Texture2D::Create(desc, pixelData.data());
	}

	void AssetManager::ProcessDirectory(const Path& _directoryPath, bool _isRecursive)
	{
		Array<Path> dirEntries = FileSystem::GetDirectoryEntries(_directoryPath);
		for (auto dirPath : dirEntries)
		{
			String ext = dirPath.GetExtensionString();
			if (ext == ".ddmeta") continue;
			if (dirPath.IsDirectory())
			{
				if (_isRecursive)
				{
					ProcessDirectory(dirPath, _isRecursive);
				}
				else
				{
					continue;
				}
			}
			else
			{
				AssetType type = GetAssetTypeFromExtension(ext);
				if (type == AssetType::None) continue;
				ProcessFile(dirPath, type);
			}
		}
	}

	void AssetManager::ProcessFile(const Path& _filePath, AssetType _assetType)
	{
		Path metaFilePath = _filePath;
		metaFilePath += ".ddmeta";
		AssetMetadata metadata = AssetMetadata();
		if (!metaFilePath.IsExist())
		{
			metadata.handle = AssetHandle::Generate();
			metadata.filePath = _filePath.ToString(); // TODO: 상대 경로로 변환해야 함
			metadata.type = _assetType;
			metadata.name = _filePath.GetFileNameWithoutExtension();
			//Create metafile
			CreateMetaDataFileInternal(metadata);
		}

		//metaFile이 존재한다는 사실이 무조건 보장
		metadata = LoadMetadata(metaFilePath);
		RegisterAsset(metadata);

		if (!metadata.IsValid())
		{
			// 메타파일이 손상되었거나 유효하지 않음
			return;
		}
	}

	void AssetManager::RegisterAsset(const AssetMetadata& _metadata)
	{
		// 레지스트리에 등록
		if (assetRegistry.find(_metadata.handle) == assetRegistry.end())
		{
			assetRegistry[_metadata.handle] = _metadata;
			assetPathMap[_metadata.filePath.ToGenericString()] = _metadata.handle;
		}
		if (!_metadata.subAssets.empty())
		{
			for (auto [key, subMetadata] : _metadata.subAssets)
			{
				RegisterAsset(subMetadata);
			}
		}
	}

	void AssetManager::CreateMetaDataFileInternal(const AssetMetadata& _metadata)
	{
		Path metaFilePath = _metadata.filePath;
		metaFilePath += ".ddmeta";

		YAML::Emitter out;
		out << YAML::BeginMap;
		// out << YAML::Key << "Handle" << YAML::Value << _metadata.Handle; (UUID를 문자열로)
		out << YAML::Key << "Handle" << YAML::Value << _metadata.handle.ToString();
		out << YAML::Key << "Path" << YAML::Value << _metadata.filePath.ToString();
		// AssetType을 문자열로 변환하는 함수가 필요함
		out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(_metadata.type);
		out << YAML::Key << "Name" << YAML::Value << _metadata.name;
		out << YAML::EndMap;

		
		std::ofstream fout(metaFilePath.ToString());
		fout << out.c_str();
		fout.close();
	}
}

