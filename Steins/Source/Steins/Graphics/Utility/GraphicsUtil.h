#pragma once

#include "Steins/Enum/RendererEnums.h"
#include "vulkan/vulkan.h"
#include "glad/glad.h"
#include "spirv_cross/spirv_cross.hpp"

#define VK_SUCCEEDED(x) x == VK_SUCCESS

namespace Steins
{
	struct DXShaderCompileParam
	{
		std::string entryPoint;
		std::string target;
	};

	struct DXShaderCompileParamW
	{
		std::wstring entryPoint;
		std::wstring target;
	};

	namespace GraphicsUtil
	{
		bool HasFlag(RenderBindFlags _target, RenderBindFlags _flags);

		UInt32 GetRenderFormatSize(RenderFormat _format);

		DXGI_FORMAT ShaderDataTypeToDXGIFormat(ShaderDataType type);

		//RenderFormat
		DXGI_FORMAT RenderFormatToDXGIFormat(RenderFormat _format);
		VkFormat RenderFormatToVkFormat(RenderFormat _format);
		VkFormat RenderFormatToGLFormat(RenderFormat _format);
		RenderFormat ConvertSPIRVTypeToRenderFormat(spirv_cross::SPIRType::BaseType _baseType, UInt32 _componentCount);


		//ShaderCompile
		String GetShaderEntryPointName(ShaderType _type);
		WideString GetShaderEntryPointNameW(ShaderType _type);
		String GetShaderTargetName(ShaderType _type, String _version);
		WideString GetShaderTargetNameW(ShaderType _type, WideString _version);

		//ShaderType, ShaderStage
		GLenum GetGLShaderStage(ShaderType _type);
		GLenum GetGLShaderType(ShaderType _type);
		D3D12_SHADER_VISIBILITY GetDX12ShaderVisibility(ShaderType _type);
		VkShaderStageFlagBits GetVKShaderStage(ShaderType _type);

		UInt32 ConvertToD3D11BindFlags(RenderBindFlags _flags);
		D3D12_RESOURCE_FLAGS ConvertToD3D12BindFlags(RenderBindFlags flags);
		D3D12_RESOURCE_STATES ConvertToD3D12ResourceStates(RenderBindFlags flags);

		VkImageUsageFlags ConvertToVkImageUsageFlags(RenderBindFlags usageFlags);

		String GetVendor(UInt32 _vendorCode);
	}
}