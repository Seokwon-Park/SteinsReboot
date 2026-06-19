#include "DaydreamPCH.h"
#include "D3D11Shader.h"

#include "D3D11RenderDevice.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"
#include "Daydream/Graphics/Utility/ShaderCompileHelper.h"

#include <spirv_cross/spirv_hlsl.hpp>

namespace Daydream
{
	D3D11Shader::D3D11Shader(D3D11RenderDevice* _device, const ShaderType& _type)
	{
		device = _device;
		shaderType = _type;
		//String target = GraphicsUtility::GetShaderTargetName(_type, "5_0");
		//String entryPoint = GraphicsUtility::GetShaderEntryPointName(_type);
		//HRESULT hr;

		//Path path(_src);
		//Array<UInt32> spirvData;
		//ShaderCompileHelper::CompileHLSLToSPIRV(path, _type);
		////spirv_cross::CompilerHLSL compiler(spirvData);
		//Unique<spirv_cross::CompilerHLSL> compiler = MakeUnique<spirv_cross::CompilerHLSL>(spirvData);
		//Unique<spirv_cross::ShaderResources> res = MakeUnique <spirv_cross::ShaderResources>(compiler->get_shader_resources());
		//for (const spirv_cross::Resource& resource : res->uniform_buffers)
		//{
		//	ShaderReflectionData sr{};
		//	sr.name = compiler->get_name(resource.id);
		//	sr.shaderResourceType = ShaderReflectionDataType::ConstantBuffer;
		//	sr.set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
		//	sr.binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
		//	sr.size = compiler->get_declared_struct_size(compiler->get_type(resource.type_id));
		//	sr.shaderType = _type;

		//	reflectionDatas.push_back(sr);
		//}

		//for (const spirv_cross::Resource& resource : res->sampled_images)
		//{
		//	ShaderReflectionData sr{};
		//	sr.name = compiler->get_name(resource.id);
		//	sr.shaderResourceType = ShaderReflectionDataType::Texture;
		//	sr.set = compiler->get_decoration(resource.id, spv::DecorationDescriptorSet);
		//	sr.binding = compiler->get_decoration(resource.id, spv::DecorationBinding);
		//	sr.shaderType = _type;

		//	const auto& type = compiler->get_type(resource.type_id);
		//	UInt32 count = 1;
		//	if (!type.array.empty())
		//	{
		//		count = type.array[0];
		//	}

		//	reflectionDatas.push_back(sr);
		//}
		//String src = ShaderCompileHelper::ConvertSPIRVtoDXBC(spirvData, _type);

		//hr = D3DCompile(src.c_str(), src.size(), nullptr, nullptr, nullptr, entryPoint.c_str(), target.c_str(), 0, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
		//DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to compile shader!");

		//hr = D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_PPV_ARGS(reflection.GetAddressOf()));
		//DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to reflection");
	}

	D3D11Shader::~D3D11Shader()
	{
	}
}
