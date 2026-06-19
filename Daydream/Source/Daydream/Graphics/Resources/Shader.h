#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Asset/Asset.h"

namespace Daydream
{


	struct ShaderReflectionData
	{
		UInt32 set; // or location
		UInt32 binding;
		UInt32 count;
		UInt64 size;
		ShaderType shaderType;
		ShaderReflectionDataType shaderResourceType;
		String name;
		RenderFormat format; // only for input layouts
	};

	struct ShaderProgram
	{

	};

	class Shader : public Asset
	{
	public:
		ASSET_CLASS_TYPE(Shader);
		virtual ~Shader() = default;

		bool LoadCache(const Path& _cachePath);
		ShaderType GetType() const { return shaderType; }

		const Array<ShaderReflectionData>& GetShaderReflectionData() const { return reflectionDatas; }

		static Shared<Shader> Create(const ShaderType& _type);
	protected:
		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) = 0;

		ShaderType shaderType;
		Array<ShaderReflectionData> reflectionDatas;
	};

}