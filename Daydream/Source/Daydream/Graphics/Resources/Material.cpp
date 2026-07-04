#include "DaydreamPCH.h"
#include "Material.h"

#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Manager/RenderCacheManager.h"
#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	Material::Material(const ShaderPipeline* _GraphicsPipelineTemplate)
	{
		textureBindingMap = _GraphicsPipelineTemplate->GetMaterialMap();
	}

	Material::~Material()
	{
		textures.clear();
		//cbuffers.clear();
		//textureCubes.clear();
	}

	void Material::SetTextureBinding(const String& _name, AssetHandle _textureHandle)
	{
		if (textureBindingMap.find(_name) == textureBindingMap.end())
		{
			//if (!_texture->HasSampler())
			//{
			//	_texture->SetSampler(ResourceManager::GetResource<Sampler>("LinearRepeat"));
			//}
			DAYDREAM_CORE_ERROR("[Material] There is no Texture Name {}", _name);
			return;
		}
		textures[_name].handle = _textureHandle;
		textures[_name].cache = AssetManager::GetAsset<Texture2D>(textures[_name].handle);
	}

	//void Material::SetTextureCube(const String& _name, Shared<TextureCube> _texture)
	//{
	//	if (materialMap.find(_name) != materialMap.end())
	//	{
	//		textureCubes[_name] = _texture;
	//	}
	//}

	//void Material::SetConstantBuffer(const String& _name, Shared<ConstantBuffer> _constantBuffer)
	//{
	//	if (materialMap.find(_name) != materialMap.end())
	//	{
	//		cbuffers[_name] = _constantBuffer;
	//	}
	//}

	Shared<Material> Material::Create(const GraphicsPipelineState* _pipeline)
	{
		return MakeShared<Material>(_pipeline->GetShaderPipeline());
	}
	Shared<Material> Material::Create(const ShaderPipeline* _shaderPipeline)
	{
		return MakeShared<Material>(_shaderPipeline);
	}
}
