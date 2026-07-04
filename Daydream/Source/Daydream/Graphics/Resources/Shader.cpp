#include "DaydreamPCH.h"
#include "Daydream/Graphics/Resources/Shader.h"

#include "Daydream/Core/Application.h"
#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{
	bool Shader::LoadCache(const Path& _cachePath)
	{
		FileReader reader(_cachePath);

		if(!reader.IsOpened())
		{ 
			DAYDREAM_CORE_ERROR("Failed to open shader cache file : {} ", _cachePath.ToString());
			return false;
		}
		
		ShaderCompileResult result;
		result.Load(reader);

		result.input.swap(input);
		result.output.swap(output);
		result.reflection.swap(reflection);
		CreateNativeShader(result.bytecode);

		reader.Close();

		for (auto& data : reflection)
		{
			data.shaderType = shaderType;
		}
	}

	Shared<Shader> Shader::Create(const ShaderType& _type)
	{
		return Renderer::GetRenderDevice()->CreateShader(_type);
	}
}
