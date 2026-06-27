#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Asset/Asset.h"

namespace Daydream
{
	struct ShaderLayoutData
	{
		String name;          
		UInt32 location;      
		RenderFormat format;  
		UInt64 size;          

		void Save(FileWriter& _writer) const
		{
			_writer.Write(name);
			_writer.Write(location);
			_writer.Write(format);
			_writer.Write(size);
		}

		void Load(FileReader& _reader)
		{
			_reader.Read(name);
			_reader.Read(location);
			_reader.Read(format);
			_reader.Read(size);
		}
	};

	struct ShaderReflectionData
	{
		String name;
		UInt32 set;
		UInt32 binding;
		UInt32 count;
		UInt64 size;
		ShaderType shaderType;
		ShaderReflectionDataType shaderResourceType;
		RenderFormat format; // only for input layouts

		void Save(FileWriter& _writer) const
		{
			_writer.Write(name);
			_writer.Write(set);
			_writer.Write(binding);
			_writer.Write(count);
			_writer.Write(shaderResourceType);
			_writer.Write(format);
		}

		void Load(FileReader& _reader)
		{
			_reader.Read(name);
			_reader.Read(set);
			_reader.Read(binding);
			_reader.Read(count);
			_reader.Read(shaderResourceType);
			_reader.Read(format);

		}
	};

	struct ShaderCompileResult
	{
		Array<ShaderLayoutData> input;
		Array<ShaderLayoutData> output;
		Array<ShaderReflectionData> reflection;
		Array<UInt8> bytecode;

		void Save(FileWriter& _writer) const
		{
			_writer.Write((UInt64)input.size());
			for (const ShaderLayoutData& data : input)
			{
				data.Save(_writer);
			}

			_writer.Write((UInt64)output.size());
			for (const ShaderLayoutData& data : output)
			{
				data.Save(_writer);
			}

			_writer.Write((UInt64)reflection.size());
			for (const ShaderReflectionData& data : reflection)
			{
				data.Save(_writer);
			}

			_writer.Write((UInt64)bytecode.size());
			_writer.Write(bytecode.data(), (UInt64)bytecode.size());
		}

		void Load(FileReader& _reader)
		{
			UInt64 inputCount = 0;
			_reader.Read(inputCount);
			input.resize(inputCount);
			for (UInt64 i = 0; i < inputCount; i++)
			{
				input[i].Load(_reader);
			}

			UInt64 outputCount = 0;
			_reader.Read(outputCount);
			output.resize(outputCount);
			for (UInt64 i = 0; i < outputCount; i++)
			{
				output[i].Load(_reader);
			}

			UInt64 reflectionCount = 0;
			_reader.Read(reflectionCount);
			reflection.resize(reflectionCount);
			for (UInt64 i = 0; i < reflectionCount; i++)
			{
				reflection[i].Load(_reader);
			}

			UInt64 byteCount = 0;
			_reader.Read(byteCount);
			bytecode.resize(byteCount);
			_reader.Read(bytecode.data(), byteCount);
		}
	};

	class Shader : public Asset
	{
	public:
		ASSET_CLASS_TYPE(Shader);
		virtual ~Shader() = default;

		bool LoadCache(const Path& _cachePath);
		ShaderType GetType() const { return shaderType; }

		const Array<ShaderLayoutData>& GetShaderInputData() const { return input; }
		const Array<ShaderLayoutData>& GetShaderOutputData() const { return output; }
		const Array<ShaderReflectionData>& GetShaderReflectionData() const { return reflection; }

		static Shared<Shader> Create(const ShaderType& _type);
	protected:
		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) = 0;

		ShaderType shaderType;
		Array<ShaderReflectionData> reflection;
		Array<ShaderLayoutData> input;
		Array<ShaderLayoutData> output;
	};

}