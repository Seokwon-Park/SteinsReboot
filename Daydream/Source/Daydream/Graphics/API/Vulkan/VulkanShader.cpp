#include "DaydreamPCH.h"
#include "VulkanShader.h"

#include "spirv_cross/spirv_cross.hpp"

#include "VulkanUtility.h"
#include "Daydream/Graphics/Utility/ShaderCompileHelper.h"


namespace Daydream
{
	VulkanShader::VulkanShader(VulkanRenderDevice* _device, const ShaderType& _type)
	{
		device = _device;
		shaderType = _type;
		stageBit = GraphicsUtility::Vulkan::ConvertToShaderStageFlagBit(_type);

		//Path path(_src);
		//switch (_mode)
		//{
		//case ShaderLoadMode::Source:
		//{
		//	DAYDREAM_CORE_ERROR("Daydream Vulkan API runtime compile is not supported!");
		//	break;
		//}
		//case ShaderLoadMode::File:
		//{
		//	std::ifstream file(_src, std::ios::ate | std::ios::binary);

		//	DAYDREAM_CORE_ASSERT(file.is_open() == true, "Failed to open file! Check directory");


		//	//tellg->파일의 입력위치 지정자를 리턴(사실상 size?)
		//	//UInt64 fileSize = (UInt64)file.tellg();
		//	Array<uint32_t> reflect;
		//	//Array<char> buffer(fileSize);
		//	//file.seekg(0);
		//	//file.read(buffer.data(), fileSize);
		//	//file.seekg(0);
		//	//file.read((char*)reflect.data(), fileSize);
		//	//file.close();

		//	ShaderCompileHelper::CompileHLSLToSPIRV(path, _type);

		//	vk::ShaderModuleCreateInfo createInfo{};
		//	createInfo.codeSize = reflect.size() * sizeof(UInt32);
		//	createInfo.pCode = reflect.data();

		//	spirv_cross::Compiler compiler(reflect);
		//	spirv_cross::ShaderResources res = compiler.get_shader_resources();

		//	if (shaderType == ShaderType::Vertex)
		//	{
		//		for (const spirv_cross::Resource& resource : res.stage_inputs)
		//		{
		//			const spirv_cross::SPIRType& spirType = compiler.get_type(resource.type_id);

		//			ShaderReflectionData sr{};
		//			sr.name = compiler.get_name(resource.id);
		//			sr.set = compiler.get_decoration(resource.id, spv::DecorationLocation);
		//			sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		//			sr.shaderResourceType = ShaderResourceType::Input;

		//			UInt32 componentCount = spirType.vecsize;
		//			spirv_cross::SPIRType::BaseType baseType = spirType.basetype;
		//			sr.format = GraphicsUtility::ConvertSPIRVTypeToRenderFormat(baseType, componentCount);
		//			sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
		//			sr.shaderType = shaderType;

		//			reflectionDatas.push_back(sr);
		//		}
		//	}


		//	for (const spirv_cross::Resource& resource : res.uniform_buffers)
		//	{
		//		ShaderReflectionData sr{};
		//		sr.name = compiler.get_name(resource.id);
		//		sr.shaderResourceType = ShaderResourceType::ConstantBuffer;
		//		sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
		//		sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
		//		sr.size = compiler.get_declared_struct_size(compiler.get_type(resource.type_id));

		//		reflectionDatas.push_back(sr);
		//	}

		//	for (const spirv_cross::Resource& resource : res.sampled_images)
		//	{
		//		ShaderReflectionData sr{};
		//		sr.name = compiler.get_name(resource.id);
		//		sr.shaderResourceType = ShaderResourceType::Texture;
		//		sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
		//		sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

		//		const auto& type = compiler.get_type(resource.type_id);
		//		UInt32 count = 1;
		//		if (!type.array.empty())
		//		{
		//			count = type.array[0];
		//		}

		//		reflectionDatas.push_back(sr);
		//	}

		//	shader = device->GetDevice().createShaderModuleUnique(createInfo);
		//	break;
		//}
		//default:
		//	break;
		//}

	}

	VulkanShader::~VulkanShader()
	{
	}

	bool VulkanShader::CreateNativeShader(const Array<UInt8>& _bytecode)
	{
		if (_bytecode.empty()) return false;
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = _bytecode.size();

		createInfo.pCode = ReinterpretCast<const UInt32*>(_bytecode.data());
		shader = device->GetDevice().createShaderModuleUnique(createInfo);

		return static_cast<bool>(shader.get());
	}



}
