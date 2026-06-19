#pragma once
#include "dxc/dxcapi.h"
#include "Daydream/Graphics/Resources//Shader.h"

namespace Daydream
{
	enum class ShaderType;

	struct ShaderCompileResult
	{
		Array<ShaderReflectionData> reflection;
		Array<UInt8> bytecode; 
	};

	class ShaderCompileHelper
	{
	public:
		static void Init();
		static void Shutdown();
		static void CompileToDXIL(const Path& _filePath, ShaderType _type, ComPtr<IDxcBlob>& _shaderBlob);
		static Array<UInt32> CompileHLSLToSPIRV(const Path& _filePath, ShaderType _type);
		static String ConvertSPIRVtoDXBC(const Array<UInt32> _spirvData, ShaderType _type);
		static String ConvertSPIRVtoGLSL(const Array<UInt32> _spirvData, ShaderType _type);
		static ShaderCompileResult CompileAndReflect(const Path& _filePath, const ShaderType& _type);
		static void SaveShaderCompileResult(const Path& _cachePath, const ShaderCompileResult& _result);

		static String GenerateShaderCacheFileName(const Path& _hlslPath);
	private:
		inline static ShaderCompileHelper* instance;

		ComPtr<IDxcUtils> utils;
		ComPtr<IDxcCompiler3> compiler;
	};
}