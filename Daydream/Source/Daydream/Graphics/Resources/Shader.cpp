#include "DaydreamPCH.h"
#include "Daydream/Graphics/Resources/Shader.h"

#include "Daydream/Core/Application.h"
#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{
	bool Shader::LoadCache(const Path& _cachePath)
	{
		File file(_cachePath);

		if(!file.OpenForRead())
		{ 
			DAYDREAM_CORE_ERROR("Failed to open shader cache file : {} ", _cachePath.ToString());
			return false;
		}
		
		UInt32 reflectionCount = 0;
		file.Read(reflectionCount);

		reflectionDatas.resize(reflectionCount);
		for (UInt32 i = 0; i < reflectionCount; i++)
		{
			auto& data = reflectionDatas[i];
			file.Read(data.set);
			file.Read(data.binding);
			file.Read(data.count);
			file.Read(data.size);
			file.Read(data.shaderType);
			file.Read(data.shaderResourceType);
			file.Read(data.format);

			// String(이름) 읽기: 길이를 먼저 읽어오고 그 길이만큼만 문자열로 빼냄
			UInt32 nameLen = 0;
			file.Read(nameLen);
			data.name.resize(nameLen);
			file.Read(data.name);
		}

		UInt32 bytecodeSize = 0;
		file.Read(bytecodeSize);

		Array<UInt8> bytecodeData;
		bytecodeData.resize(bytecodeSize);
		file.Read(bytecodeData);

		file.Close();

		CreateNativeShader(bytecodeData);
	}

	Shared<Shader> Shader::Create(const ShaderType& _type)
	{
		return Renderer::GetRenderDevice()->CreateShader(_type);
	}
}
