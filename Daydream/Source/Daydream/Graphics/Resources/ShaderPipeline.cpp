#include "DaydreamPCH.h"
#include "ShaderPipeline.h"

#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	const Array<Shader*>& ShaderPipeline::GetShaders() const
	{
		return shaders;
	}

	Shared<ShaderPipeline> ShaderPipeline::Create(const ShaderPipelineDesc& _desc)
	{
		Shared<ShaderPipeline> shaderPipeline = MakeShared<ShaderPipeline>(_desc);

		shaderPipeline->SetIOLayoutData();
		shaderPipeline->CreateShaderBindingMap();
		shaderPipeline->CreateMaterialMap();

		return shaderPipeline;
	}

	const ShaderReflectionData* ShaderPipeline::GetShaderBindingInfo(const String& _name) const
	{
		auto itr = shaderBindingMap.find(_name);

		if (itr == shaderBindingMap.end())
		{
			DAYDREAM_CORE_WARN("{} is not valid ShaderResource Name!", _name);
			return nullptr;
		}

		return &itr->second;
	}

	ShaderPipeline::ShaderPipeline(const ShaderPipelineDesc& _desc)
	{
		DAYDREAM_CORE_ASSERT(_desc.vertexShader, "VS can't be nullptr!");

		vertexShader = _desc.vertexShader;
		hullShader = _desc.hullShader;
		domainShader = _desc.domainShader;
		geometryShader = _desc.geometryShader;
		pixelShader = _desc.pixelShader;

		shaders.push_back(vertexShader);
		if (hullShader != nullptr) shaders.push_back(hullShader);
		if (domainShader != nullptr) shaders.push_back(domainShader);
		if (geometryShader != nullptr) shaders.push_back(geometryShader);
		if (pixelShader != nullptr) shaders.push_back(pixelShader);


		rasterizerState = _desc.rasterizerState;
		depthStencilState = _desc.depthStencilState;
		blendState = _desc.blendState;
	}

	void ShaderPipeline::SetIOLayoutData()
	{
		pipelineInputData = vertexShader->GetShaderInputData();
		if (pixelShader != nullptr) pipelineOutputData = pixelShader->GetShaderOutputData();
	}

	void ShaderPipeline::CreateShaderBindingMap()
	{
		int descriptorTableIndex = 0;
		for (const Shader* shader : shaders)
		{
			for (ShaderReflectionData data : shader->GetShaderReflectionData())
			{
				shaderBindingMap.insert({ data.name, data });
				setCount = Math::Max(setCount, data.set + 1);
			}
		}
	}

	void ShaderPipeline::CreateMaterialMap()
	{
		for (const auto& [name, data] : shaderBindingMap)
		{
			if (data.name.find("mat_") != String::npos)
			{
				materialMap.insert({ data.name,data.shaderResourceType });
			}
		}
	}


	Shader* ShaderPipeline::GetShader(ShaderType _type) const
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
}