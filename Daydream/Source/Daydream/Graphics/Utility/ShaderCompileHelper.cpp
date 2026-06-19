#include "DaydreamPCH.h"
#include "ShaderCompileHelper.h"
#include "Daydream/Enum/RendererEnums.h"

#include "Daydream/Graphics/Core/Renderer.h"

#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

#include <Daydream/Graphics/Utility/GraphicsUtility.h>

#include <Daydream/Graphics/Resources/Shader.h>

namespace Daydream
{
	namespace
	{
		RenderFormat ConvertToRenderFormat(const D3D12_SIGNATURE_PARAMETER_DESC& paramDesc)
		{
			int componentCount = Math::BitCount(paramDesc.Mask); // 비트 수 세는 함수

			switch (componentCount)
			{
			case 1:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 2:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 3:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 4:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32A32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32A32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32A32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			default:
				return RenderFormat::UNKNOWN;
			}
		}

		RenderFormat ConvertToRenderFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
		{
			int componentCount = Math::BitCount(paramDesc.Mask); // 비트 수 세는 함수

			switch (componentCount)
			{
			case 1:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 2:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 3:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 4:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32A32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32A32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32A32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			default:
				return RenderFormat::UNKNOWN;
			}
		}
	}

	void ShaderCompileHelper::Init()
	{
		instance = new ShaderCompileHelper();
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(instance->utils.GetAddressOf()));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(instance->compiler.GetAddressOf()));
	}

	void ShaderCompileHelper::Shutdown()
	{
		delete instance;
		//utils->Release();
		//utils = nullptr;
		//compiler->Release();
		//compiler = nullptr;
	}
	void ShaderCompileHelper::CompileToDXIL(const Path& _filePath, ShaderType _type, ComPtr<IDxcBlob>& _shaderBlob)
	{
		ComPtr<IDxcBlobEncoding> sourceBlob;
		HRESULT hr = instance->utils->LoadFile(_filePath.ToWString().c_str(), nullptr, sourceBlob.GetAddressOf());
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to load File! {0}", _filePath.ToString());

		// 2. DxcBuffer 설정
		DxcBuffer sourceBuffer{};
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		// 3. 컴파일 인자 구성
		std::vector<LPCWSTR> args;

		WideString target = GraphicsUtility::GetShaderTargetNameW(_type, L"6_0");
		WideString entryPoint = GraphicsUtility::GetShaderEntryPointNameW(_type);

		// 기본 옵션
		args.push_back(L"-T");
		args.push_back(target.c_str());
		args.push_back(L"-E");
		args.push_back(entryPoint.c_str());
		args.push_back(L"-O3");
		args.push_back(L"-Wno-ignored-attributes");

		ComPtr<IDxcResult> result;
		hr = instance->compiler->Compile(&sourceBuffer,
			args.data(),
			static_cast<UINT32>(args.size()),
			nullptr,  // 인클루드 핸들러 없음
			IID_PPV_ARGS(&result));

		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to compile shader!");

		// 에러 확인
		ComPtr<IDxcBlobUtf8> errors;
		result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			String errorMsg = errors->GetStringPointer();

			DAYDREAM_CORE_ERROR("{0}", errorMsg);

			HRESULT status;
			result->GetStatus(&status);
			if (FAILED(status)) return;
		}

		// SPIR-V 결과
		hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(_shaderBlob.GetAddressOf()), nullptr);
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to compile shader!");
	}

	Array<UInt32> ShaderCompileHelper::CompileHLSLToSPIRV(const Path& _filePath, ShaderType _type)
	{
		Array<UInt32> output;

		ComPtr<IDxcBlobEncoding> sourceBlob;
		HRESULT hr = instance->utils->LoadFile(_filePath.ToWString().c_str(), nullptr, sourceBlob.GetAddressOf());
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to load File! {0}", _filePath.ToString());

		// 2. DxcBuffer 설정
		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = DXC_CP_ACP; // LoadFile이 자동으로 인코딩 감지

		// 3. 컴파일 인자 구성
		std::vector<LPCWSTR> args;

		WideString target = GraphicsUtility::GetShaderTargetNameW(_type, L"6_0");
		WideString entryPoint = GraphicsUtility::GetShaderEntryPointNameW(_type);

		// 기본 옵션
		args.push_back(L"-T");
		args.push_back(target.c_str());
		args.push_back(L"-E");
		args.push_back(entryPoint.c_str());
		args.push_back(L"-spirv");
		args.push_back(L"-fspv-reflect");
		if (_type == ShaderType::Vertex)
		{
			args.push_back(L"-fvk-invert-y");
		}
		args.push_back(L"-fvk-use-dx-layout");
		args.push_back(L"-O0");

		ComPtr<IDxcResult> result;
		hr = instance->compiler->Compile(&sourceBuffer,
			args.data(),
			static_cast<UINT32>(args.size()),
			nullptr,  // 인클루드 핸들러 없음
			IID_PPV_ARGS(&result));

		if (FAILED(hr)) return output;

		// 에러 확인
		ComPtr<IDxcBlobUtf8> errors;
		result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (errors && errors->GetStringLength() > 0)
		{
			String errorMsg = errors->GetStringPointer();

			DAYDREAM_CORE_ERROR("{0}", errorMsg);

			HRESULT status;
			result->GetStatus(&status);
			if (FAILED(status)) return output;
		}

		// SPIR-V 결과
		ComPtr<IDxcBlob> spirvBlob;
		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirvBlob), nullptr);
		if (!spirvBlob)
		{
			DAYDREAM_CORE_ERROR("Fuck");
			return output;
		}

		output.resize((spirvBlob->GetBufferSize() + sizeof(UInt32) - 1) / sizeof(UInt32));
		memcpy(output.data(), spirvBlob->GetBufferPointer(), spirvBlob->GetBufferSize());

		return output;
	}

	String ShaderCompileHelper::ConvertSPIRVtoDXBC(const Array<UInt32> _spirvData, ShaderType _type)
	{
		String hlslSource;
		try
		{
			spirv_cross::CompilerHLSL hlsl(_spirvData);

			spirv_cross::CompilerGLSL::Options commonOptions{};
			commonOptions.vertex.flip_vert_y = true;

			hlsl.set_common_options(commonOptions);

			spirv_cross::CompilerHLSL::Options options{};
			options.shader_model = 50;
			options.use_entry_point_name = true;


			hlsl.set_hlsl_options(options);

			hlslSource = hlsl.compile();
		}
		catch (const spirv_cross::CompilerError& e)
		{
			std::cerr << "SPIRV-Cross Error: " << e.what() << std::endl;
		}
		return hlslSource;
	}
	String ShaderCompileHelper::ConvertSPIRVtoGLSL(const Array<UInt32> _spirvData, ShaderType _type)
	{
		String glslSource;

		try
		{
			spirv_cross::CompilerGLSL compilerGLSL(_spirvData);

			// GLSL 컴파일 옵션 설정
			spirv_cross::CompilerGLSL::Options options{};
			options.version = 450;
			options.separate_shader_objects = true;
			options.vertex.flip_vert_y = true;

			compilerGLSL.set_common_options(options);
			// GLSL 코드 생성
			glslSource = compilerGLSL.compile();
		}
		catch (const spirv_cross::CompilerError& e)
		{
			std::cerr << "SPIRV-Cross Error: " << e.what() << std::endl;
		}
		return glslSource;
	}

	ShaderCompileResult ShaderCompileHelper::CompileAndReflect(const Path& _filePath, const ShaderType& _type)
	{
		ShaderCompileResult result{};
		// 공통 HLSL -> SPIR-V로 변환
		Array<UInt32> spirvData = CompileHLSLToSPIRV(_filePath, _type);
		DAYDREAM_CORE_ASSERT(!spirvData.empty(), "Failed To Compile Shader! : {0}", _filePath.ToString());
		spirv_cross::Compiler compiler(spirvData);
		spirv_cross::ShaderResources res = compiler.get_shader_resources();

		switch (Renderer::GetAPI())
		{
		case RendererAPIType::Vulkan:
		{
			result.bytecode.resize(spirvData.size() * sizeof(UInt32));
			std::memcpy(result.bytecode.data(), spirvData.data(), result.bytecode.size());

			if (_type == ShaderType::Vertex)
			{
				for (const spirv_cross::Resource& resource : res.stage_inputs)
				{
					const spirv_cross::SPIRType& spirType = compiler.get_type(resource.type_id);

					ShaderReflectionData sr{};
					sr.name = compiler.get_name(resource.id);
					sr.set = compiler.get_decoration(resource.id, spv::DecorationLocation);
					sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
					sr.shaderResourceType = ShaderReflectionDataType::Input;

					UInt32 componentCount = spirType.vecsize;
					spirv_cross::SPIRType::BaseType baseType = spirType.basetype;
					sr.format = GraphicsUtility::ConvertSPIRVTypeToRenderFormat(baseType, componentCount);
					sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
					sr.shaderType = _type;

					result.reflection.push_back(sr);
				}
			}
			break;
		}
#ifdef DAYDREAM_PLATFORM_WINDOWS
		case RendererAPIType::DirectX12:
		{
			ComPtr<IDxcBlob> dxilBlob;
			ComPtr<ID3D12ShaderReflection> reflection;
			CompileToDXIL(_filePath, _type, dxilBlob);

			result.bytecode.resize(dxilBlob->GetBufferSize());
			std::memcpy(result.bytecode.data(), dxilBlob->GetBufferPointer(), dxilBlob->GetBufferSize());

			// Reflection Input Layout
			DxcBuffer reflectionBuffer = {};
			reflectionBuffer.Ptr = dxilBlob->GetBufferPointer();
			reflectionBuffer.Size = dxilBlob->GetBufferSize();

			HRESULT hr = instance->utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(reflection.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create reflection!");

			D3D12_SHADER_DESC shaderDesc;
			hr = reflection->GetDesc(&shaderDesc);
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to get shader description");

			// 각 입력 파라미터에 대해 순회
			if (_type == ShaderType::Vertex)
			{
				for (UInt32 i = 0; i < shaderDesc.InputParameters; i++)
				{
					D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
					reflection->GetInputParameterDesc(i, &paramDesc);

					ShaderReflectionData sr{};
					sr.name = paramDesc.SemanticName;
					sr.set = 0;
					sr.binding = paramDesc.SemanticIndex;
					sr.shaderResourceType = ShaderReflectionDataType::Input;
					sr.format = ConvertToRenderFormat(paramDesc);
					sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
					sr.shaderType = _type;

					result.reflection.push_back(sr);
				}
			}
			break;
		}
		case RendererAPIType::DirectX11:
		{
			String target = GraphicsUtility::GetShaderTargetName(_type, "5_0");
			String entryPoint = GraphicsUtility::GetShaderEntryPointName(_type);

			String src = ConvertSPIRVtoDXBC(spirvData, _type);

			ComPtr<ID3DBlob> shaderBlob;
			ComPtr<ID3DBlob> errorBlob;
			HRESULT hr = D3DCompile(src.c_str(), src.size(), nullptr, nullptr, nullptr, entryPoint.c_str(), target.c_str(), 0, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

			if (FAILED(hr))
			{
				DAYDREAM_CORE_ERROR("DX11 Compile Failed: {0}", (char*)errorBlob->GetBufferPointer());
				return result;
			}

			result.bytecode.resize(shaderBlob->GetBufferSize());
			std::memcpy(result.bytecode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

			// Reflection Input Layout
			ComPtr<ID3D11ShaderReflection> reflection;

			hr = D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_PPV_ARGS(reflection.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to reflection");

			// 셰이더 입력 시그니처 정보 얻기
			D3D11_SHADER_DESC shaderDesc;
			reflection->GetDesc(&shaderDesc);

			if (_type == ShaderType::Vertex)
			{
				// 각 입력 파라미터에 대해 순회
				for (UINT i = 0; i < shaderDesc.InputParameters; i++)
				{
					D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
					reflection->GetInputParameterDesc(i, &paramDesc);

					ShaderReflectionData sr{};
					sr.name = paramDesc.SemanticName;
					sr.set = 0;
					sr.binding = paramDesc.SemanticIndex;
					sr.shaderResourceType = ShaderReflectionDataType::Input;
					sr.format = ConvertToRenderFormat(paramDesc);
					sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
					sr.shaderType = _type;

					result.reflection.push_back(sr);
				}
			}
			break;
		}
#endif // DAYDREAM_PLATFORM_WINDOWS
		case RendererAPIType::OpenGL:
		{
			String glsl = ConvertSPIRVtoGLSL(spirvData, _type);

			// 문자열 nullptr 포함
			result.bytecode.resize(glsl.length() + 1);
			std::memcpy(result.bytecode.data(), glsl.c_str(), result.bytecode.size());

			if (_type == ShaderType::Vertex)
			{
				for (const spirv_cross::Resource& resource : res.stage_inputs)
				{
					const spirv_cross::SPIRType& spirType = compiler.get_type(resource.type_id);

					ShaderReflectionData sr{};
					sr.name = compiler.get_name(resource.id);
					sr.set = compiler.get_decoration(resource.id, spv::DecorationLocation);
					sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
					sr.shaderResourceType = ShaderReflectionDataType::Input;

					UInt32 componentCount = spirType.vecsize;
					spirv_cross::SPIRType::BaseType baseType = spirType.basetype;
					sr.format = GraphicsUtility::ConvertSPIRVTypeToRenderFormat(baseType, componentCount);
					sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
					sr.shaderType = _type;

					result.reflection.push_back(sr);
				}
			}
			break;
		}
		}

		for (const spirv_cross::Resource& resource : res.uniform_buffers)
		{
			ShaderReflectionData sr{};
			sr.name = compiler.get_name(resource.id);
			sr.shaderResourceType = ShaderReflectionDataType::ConstantBuffer;
			sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			sr.size = compiler.get_declared_struct_size(compiler.get_type(resource.type_id));
			sr.shaderType = _type;


			result.reflection.push_back(sr);
		}

		for (const spirv_cross::Resource& resource : res.sampled_images)
		{
			ShaderReflectionData sr{};
			sr.name = compiler.get_name(resource.id);
			sr.shaderResourceType = ShaderReflectionDataType::Texture;
			sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			sr.shaderType = _type;

			const auto& type = compiler.get_type(resource.type_id);
			UInt32 count = 1;
			if (!type.array.empty())
			{
				count = type.array[0];
			}
			sr.count = count;

			result.reflection.push_back(sr);
		}

		return result;
	}

	void ShaderCompileHelper::SaveShaderCompileResult(const Path& _cachePath, const ShaderCompileResult& _result)
	{
		File file(_cachePath);

		file.OpenForWrite();

		UInt32 reflectionCount = (UInt32)_result.reflection.size();
		file.Write(reflectionCount); // 배열에 몇 개 들어있는지 갯수 저장
		for (const auto& data : _result.reflection)
		{
			file.Write(data.set);
			file.Write(data.binding);
			file.Write(data.count);
			file.Write(data.size);
			file.Write(data.shaderType);
			file.Write(data.shaderResourceType);
			file.Write(data.format);
			UInt32 nameLen = (UInt32)data.name.length();
			file.Write(nameLen);
			file.Write(data.name);
		}

		UInt32 bytecodeSize = (UInt32)_result.bytecode.size();
		file.Write(bytecodeSize);
		file.Write(_result.bytecode);

		file.Close();

	}

	String ShaderCompileHelper::GenerateShaderCacheFileName(const Path& _hlslPath)
	{
		String result = _hlslPath.GetFileNameWithoutExt();
		return result + "_" + GraphicsUtility::GetAPIString(Renderer::GetAPI()) + ".ddshader";
	}
}
