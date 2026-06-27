#pragma once
#include "dxc/dxcapi.h"
#include "Daydream/Graphics/Resources/Shader.h"

#ifdef DAYDREAM_PLATFORM_WINDOWS
#include <wrl/client.h>
template<typename T>
using DxcComPtr = Microsoft::WRL::ComPtr<T>;
#else
// 리눅스/맥 빌드 시에는 DXC가 흉내 내서 제공하는 호환용 스마트 포인터 사용
#include <dxc/Support/WinAdapter.h> // (또는 microcom.h 등 DXC 제공 헤더)
template<typename T>
using DxcComPtr = CComPtr<T>;
#endif

namespace Daydream
{
	enum class ShaderType;

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
		inline static DxcComPtr<IDxcUtils> utils = nullptr;
		inline static DxcComPtr<IDxcCompiler3> compiler = nullptr;
	};
}