#include "DaydreamPCH.h"
#include "ShaderGroup.h"

#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	const Array<Shader*>& ShaderGroup::GetShaders() const
	{
		return shaders;
	}

	Shared<ShaderGroup> ShaderGroup::Create(Shader* _vertexShader)
	{
		return Create(_vertexShader, nullptr);
	}

	Shared<ShaderGroup> ShaderGroup::Create(Shader* _vertexShader, Shader* _pixelShader)
	{
		return Create(_vertexShader, nullptr, _pixelShader);
	}

	Shared<ShaderGroup> ShaderGroup::Create(Shader* _vertexShader, Shader* _geometryShader, Shader* _pixelShader)
	{
		return Create(_vertexShader, nullptr, nullptr, _geometryShader, _pixelShader);
	}
	Shared<ShaderGroup> ShaderGroup::Create(Shader* _vertexShader, Shader* _hullShader, Shader* _domainShader, Shader* _geometryShader, Shader* _pixelShader)
	{
		Shared<ShaderGroup> shaderGroup = Shared<ShaderGroup>(new ShaderGroup(
			_vertexShader,
			_hullShader,
			_domainShader,
			_geometryShader,
			_pixelShader
		));

		shaderGroup->CreateInputReflectionData();
		shaderGroup->CreateShaderBindingMap();
		shaderGroup->CreateMaterialMap();

		return shaderGroup;
	}

	const ShaderReflectionData* ShaderGroup::GetShaderBindingInfo(const String& _name) const
	{
		auto itr = shaderBindingMap.find(_name);

		if (itr == shaderBindingMap.end())
		{
			DAYDREAM_CORE_WARN("{} is not valid ShaderResource Name!", _name);
			return nullptr;
		}

		return &itr->second;
	}

	ShaderGroup::ShaderGroup(Shader* _vertexShader, Shader* _hullShader, Shader* _domainShader, Shader* _geometryShader, Shader* _pixelShader)
	{
		DAYDREAM_CORE_ASSERT(_vertexShader, "VS can't be nullptr!");

		vertexShader = _vertexShader;
		hullShader = _hullShader;
		domainShader = _domainShader;
		geometryShader = _geometryShader;
		pixelShader = _pixelShader;

		shaders.push_back(vertexShader);
		if (hullShader != nullptr) shaders.push_back(hullShader);
		if (domainShader != nullptr) shaders.push_back(domainShader);
		if (geometryShader != nullptr) shaders.push_back(geometryShader);
		if (pixelShader != nullptr) shaders.push_back(pixelShader);
	}

	void ShaderGroup::CreateInputReflectionData()
	{
		for (const ShaderReflectionData& data : vertexShader->GetShaderReflectionData())
		{
			if (data.shaderResourceType == ShaderReflectionDataType::Input)
			{
				inputReflectionData.push_back(data);
			}
		}
	}

	void ShaderGroup::CreateShaderBindingMap()
	{
		int descriptorTableIndex = 0;
		for (const Shader* shader : shaders)
		{
			for (ShaderReflectionData data : shader->GetShaderReflectionData())
			{
				if (data.shaderResourceType != ShaderReflectionDataType::Input)
				{
					shaderBindingMap.insert({ data.name, data });
					setCount = Math::Max(setCount, data.set+1);
				}
			}
		}
	}

	void ShaderGroup::CreateMaterialMap()
	{
		for (const auto& [name, data] : shaderBindingMap)
		{
			if (data.name.find("mat_") != String::npos)
			{
				materialMap.insert({ data.name,data.shaderResourceType });
			}
		}
	}


	Shader* ShaderGroup::GetShader(ShaderType _type) const
	{
		switch (_type)
		{
		case ShaderType::None:
			break;
		case ShaderType::Vertex:
			return vertexShader;
			break;
		case ShaderType::Hull:
			DAYDREAM_CORE_WARN("HS is nullptr!");
			return hullShader;
			break;
		case ShaderType::Domain:
			DAYDREAM_CORE_WARN("DS is nullptr!");
			return domainShader;
			break;
		case ShaderType::Geometry:
			DAYDREAM_CORE_WARN("GS is nullptr!");
			return geometryShader;
			break;
		case ShaderType::Pixel:
			//DAYDREAM_CORE_WARN("PS is nullptr!");
			return pixelShader;
			break;
		case ShaderType::Compute:
			break;
		default:
			break;
		}
		DAYDREAM_CORE_ASSERT(false, "Unknown Shader Type!");
		return nullptr;
	}
	Shared<ShaderGroup> ShaderGroup::Create(const Path& _vertexShaderPath, const Path&
		_pixelShaderPath)
	{
		auto vertexShader = AssetManager::GetAssetByPath<Shader>(_vertexShaderPath);
		auto pixelShader = AssetManager::GetAssetByPath<Shader>(_pixelShaderPath);
		return Create(vertexShader, pixelShader);
	}

	Shared<ShaderGroup> ShaderGroup::CreateBuiltin(const Path& _vertexShaderName)
	{
		const Path BuiltinShaderDir = "Asset/Shader/";

		Path vertexPath = BuiltinShaderDir / _vertexShaderName;

		auto vertexShader = AssetManager::GetAssetByPath<Shader>(vertexPath);
		return Create(vertexShader);
	}

	Shared<ShaderGroup> ShaderGroup::CreateBuiltin(const Path& _vertexShaderName, const Path& _pixelShaderName)
	{
		const Path BuiltinShaderDir = "Asset/Shader/";

		Path vertexPath = BuiltinShaderDir / _vertexShaderName;
		Path pixelPath = BuiltinShaderDir / _pixelShaderName;

		auto vertexShader = AssetManager::GetAssetByPath<Shader>(vertexPath);
		auto pixelShader = AssetManager::GetAssetByPath<Shader>(pixelPath);
		return Create(vertexShader, pixelShader);
	}
}