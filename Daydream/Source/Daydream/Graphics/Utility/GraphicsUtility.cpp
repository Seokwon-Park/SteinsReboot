#include "DaydreamPCH.h"
#include "GraphicsUtility.h"

#include "Daydream/Graphics/Core/Renderer.h"

#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Framebuffer.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
	namespace GraphicsUtility
	{
		UInt32 GraphicsUtility::GetRenderFormatSize(RenderFormat _format)
		{
			switch (_format)
			{
				// 128-bit formats (16 bytes)
			case RenderFormat::R32G32B32A32_FLOAT:
			case RenderFormat::R32G32B32A32_UINT:
			case RenderFormat::R32G32B32A32_SINT:
				return 16;

				// 96-bit formats (12 bytes)
			case RenderFormat::R32G32B32_FLOAT:
			case RenderFormat::R32G32B32_UINT:
			case RenderFormat::R32G32B32_SINT:
				return 12;

				// 64-bit formats (8 bytes)
			case RenderFormat::R16G16B16A16_FLOAT:
			case RenderFormat::R16G16B16A16_UNORM:
			case RenderFormat::R16G16B16A16_UINT:
			case RenderFormat::R16G16B16A16_SINT:
			case RenderFormat::R32G32_FLOAT:
			case RenderFormat::R32G32_UINT:
			case RenderFormat::R32G32_SINT:
			case RenderFormat::D32_FLOAT_S8X24_UINT:
				return 8;

				// 32-bit formats (4 bytes)
			case RenderFormat::R10G10B10A2_UNORM:
			case RenderFormat::R10G10B10A2_UINT:
			case RenderFormat::R11G11B10_FLOAT:
			case RenderFormat::R8G8B8A8_UNORM:
			case RenderFormat::R8G8B8A8_UNORM_SRGB:
			case RenderFormat::R8G8B8A8_UINT:
			case RenderFormat::R8G8B8A8_SINT:
			case RenderFormat::R16G16_FLOAT:
			case RenderFormat::R16G16_UNORM:
			case RenderFormat::R16G16_UINT:
			case RenderFormat::R16G16_SINT:
			case RenderFormat::D32_FLOAT:
			case RenderFormat::R32_FLOAT:
			case RenderFormat::R32_UINT:
			case RenderFormat::R32_SINT:
			case RenderFormat::D24_UNORM_S8_UINT:
			case RenderFormat::B8G8R8A8_UNORM:
			case RenderFormat::B8G8R8A8_UNORM_SRGB:
			case RenderFormat::B8G8R8X8_UNORM:
			case RenderFormat::R9G9B9E5_SHAREDEXP:
				return 4;

				// 16-bit formats (2 bytes)
			case RenderFormat::R8G8_UNORM:
			case RenderFormat::R8G8_UINT:
			case RenderFormat::R8G8_SINT:
			case RenderFormat::R16_FLOAT:
			case RenderFormat::D16_UNORM:
			case RenderFormat::R16_UNORM:
			case RenderFormat::R16_UINT:
			case RenderFormat::R16_SINT:
			case RenderFormat::B5G6R5_UNORM:
			case RenderFormat::B5G5R5A1_UNORM:
			case RenderFormat::B4G4R4A4_UNORM:
			case RenderFormat::A4B4G4R4_UNORM:
				return 2;

				// 8-bit formats (1 byte)
			case RenderFormat::R8_UNORM:
			case RenderFormat::R8_UINT:
			case RenderFormat::R8_SINT:
			case RenderFormat::A8_UNORM:
				return 1;

				// Compressed formats (variable size, but commonly used block sizes)
			case RenderFormat::BC1_UNORM:
			case RenderFormat::BC1_UNORM_SRGB:
			case RenderFormat::BC4_UNORM:
			case RenderFormat::BC4_SNORM:
				return 8; // 8 bytes per 4x4 block

			case RenderFormat::BC2_UNORM:
			case RenderFormat::BC2_UNORM_SRGB:
			case RenderFormat::BC3_UNORM:
			case RenderFormat::BC3_UNORM_SRGB:
			case RenderFormat::BC5_UNORM:
			case RenderFormat::BC5_SNORM:
			case RenderFormat::BC6H_UF16:
			case RenderFormat::BC6H_SF16:
			case RenderFormat::BC7_UNORM:
			case RenderFormat::BC7_UNORM_SRGB:
				return 16; // 16 bytes per 4x4 block

				// Special/YUV formats (commonly used sizes)
			case RenderFormat::NV12:
				return 1; // Variable, but 12 bits per pixel average

			case RenderFormat::YUY2:
				return 2; // 16 bits per pixel

				// Unknown or unsupported format
			case RenderFormat::UNKNOWN:
			default:
				return 0;
			}
		}

		UInt32 GetRenderFormatCount(RenderFormat _format)
		{
			switch (_format)
			{
			case RenderFormat::R32G32B32A32_FLOAT:
			case RenderFormat::R32G32B32A32_UINT:
			case RenderFormat::R32G32B32A32_SINT:
			case RenderFormat::R16G16B16A16_FLOAT:
			case RenderFormat::R16G16B16A16_UNORM:
			case RenderFormat::R16G16B16A16_UINT:
			case RenderFormat::R16G16B16A16_SINT:
			case RenderFormat::R10G10B10A2_UNORM:
			case RenderFormat::R10G10B10A2_UINT:
			case RenderFormat::R8G8B8A8_UNORM:
			case RenderFormat::R8G8B8A8_UNORM_SRGB:
			case RenderFormat::R8G8B8A8_UINT:
			case RenderFormat::R8G8B8A8_SINT:
			case RenderFormat::B8G8R8A8_UNORM:
			case RenderFormat::B8G8R8A8_UNORM_SRGB:
			case RenderFormat::B8G8R8X8_UNORM:
			case RenderFormat::B5G5R5A1_UNORM:
			case RenderFormat::B4G4R4A4_UNORM:
			case RenderFormat::A4B4G4R4_UNORM:
				return 4;
			case RenderFormat::R32G32B32_FLOAT:
			case RenderFormat::R32G32B32_UINT:
			case RenderFormat::R32G32B32_SINT:
			case RenderFormat::R11G11B10_FLOAT:
			case RenderFormat::R9G9B9E5_SHAREDEXP:
			case RenderFormat::B5G6R5_UNORM:
				return 3;
			case RenderFormat::R32G32_FLOAT:
			case RenderFormat::R32G32_UINT:
			case RenderFormat::R32G32_SINT:
			case RenderFormat::R16G16_FLOAT:
			case RenderFormat::R16G16_UNORM:
			case RenderFormat::R16G16_UINT:
			case RenderFormat::R16G16_SINT:
			case RenderFormat::R8G8_UNORM:
			case RenderFormat::R8G8_UINT:
			case RenderFormat::R8G8_SINT:
			case RenderFormat::D32_FLOAT_S8X24_UINT:
			case RenderFormat::D24_UNORM_S8_UINT:
				return 2;
			case RenderFormat::R32_FLOAT:
			case RenderFormat::R32_UINT:
			case RenderFormat::R32_SINT:
			case RenderFormat::D32_FLOAT:
			case RenderFormat::R16_FLOAT:
			case RenderFormat::R16_UNORM:
			case RenderFormat::R16_UINT:
			case RenderFormat::R16_SINT:
			case RenderFormat::D16_UNORM:
			case RenderFormat::R8_UNORM:
			case RenderFormat::R8_UINT:
			case RenderFormat::R8_SINT:
			case RenderFormat::A8_UNORM:
				return 1;
			case RenderFormat::BC1_UNORM:
			case RenderFormat::BC1_UNORM_SRGB:
			case RenderFormat::BC4_UNORM:
			case RenderFormat::BC4_SNORM:
			case RenderFormat::BC2_UNORM:
			case RenderFormat::BC2_UNORM_SRGB:
			case RenderFormat::BC3_UNORM:
			case RenderFormat::BC3_UNORM_SRGB:
			case RenderFormat::BC5_UNORM:
			case RenderFormat::BC5_SNORM:
			case RenderFormat::BC6H_UF16:
			case RenderFormat::BC6H_SF16:
			case RenderFormat::BC7_UNORM:
			case RenderFormat::BC7_UNORM_SRGB:
			case RenderFormat::NV12:
			case RenderFormat::YUY2:
			case RenderFormat::UNKNOWN:
			default:
				return 0;
			}
		}

		//VkFormat GraphicsUtil::ConvertRenderFormatToVkFormat(RenderFormat _format)
		//{
		//	switch (_format)
		//	{
		//	case RenderFormat::UNKNOWN:							return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R32G32B32A32_TYPELESS:			return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R32G32B32A32_FLOAT:				return VK_FORMAT_R32G32B32A32_SFLOAT;
		//	case RenderFormat::R32G32B32A32_UINT:				return VK_FORMAT_R32G32B32A32_UINT;
		//	case RenderFormat::R32G32B32A32_SINT:				return VK_FORMAT_R32G32B32A32_SINT;
		//	case RenderFormat::R32G32B32_TYPELESS:				return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R32G32B32_FLOAT:					return VK_FORMAT_R32G32B32_SFLOAT;
		//	case RenderFormat::R32G32B32_UINT:					return VK_FORMAT_R32G32B32_UINT;
		//	case RenderFormat::R32G32B32_SINT:					return VK_FORMAT_R32G32B32_SINT;
		//	case RenderFormat::R16G16B16A16_TYPELESS:			return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R16G16B16A16_FLOAT:				return VK_FORMAT_R16G16B16A16_SFLOAT;
		//	case RenderFormat::R16G16B16A16_UNORM:				return VK_FORMAT_R16G16B16A16_UNORM;
		//	case RenderFormat::R16G16B16A16_UINT:				return VK_FORMAT_R16G16B16A16_UINT;
		//	case RenderFormat::R16G16B16A16_SNORM:				return VK_FORMAT_R16G16B16A16_SNORM;
		//	case RenderFormat::R16G16B16A16_SINT:				return VK_FORMAT_R16G16B16A16_SINT;
		//	case RenderFormat::R32G32_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R32G32_FLOAT:					return VK_FORMAT_R32G32_SFLOAT;
		//	case RenderFormat::R32G32_UINT:						return VK_FORMAT_R32G32_UINT;
		//	case RenderFormat::R32G32_SINT:						return VK_FORMAT_R32G32_SINT;
		//	case RenderFormat::R32G8X24_TYPELESS:				return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::D32_FLOAT_S8X24_UINT:			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		//	case RenderFormat::R32_FLOAT_X8X24_TYPELESS:		return VK_FORMAT_D32_SFLOAT_S8_UINT;
		//	case RenderFormat::X32_TYPELESS_G8X24_UINT:			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		//	case RenderFormat::R10G10B10A2_TYPELESS:			return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R10G10B10A2_UNORM:				return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		//	case RenderFormat::R10G10B10A2_UINT:				return VK_FORMAT_A2B10G10R10_UINT_PACK32;
		//	case RenderFormat::R11G11B10_FLOAT:					return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		//	case RenderFormat::R8G8B8A8_TYPELESS:				return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R8G8B8A8_UNORM:					return VK_FORMAT_R8G8B8A8_UNORM;
		//	case RenderFormat::R8G8B8A8_UNORM_SRGB:				return VK_FORMAT_R8G8B8A8_SRGB;
		//	case RenderFormat::R8G8B8A8_UINT:					return VK_FORMAT_R8G8B8A8_UINT;
		//	case RenderFormat::R8G8B8A8_SNORM:					return VK_FORMAT_R8G8B8A8_SNORM;
		//	case RenderFormat::R8G8B8A8_SINT:					return VK_FORMAT_R8G8B8A8_SINT;
		//	case RenderFormat::R16G16_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R16G16_FLOAT:					return VK_FORMAT_R16G16_SFLOAT;
		//	case RenderFormat::R16G16_UNORM:					return VK_FORMAT_R16G16_UNORM;
		//	case RenderFormat::R16G16_UINT:						return VK_FORMAT_R16G16_UINT;
		//	case RenderFormat::R16G16_SNORM:					return VK_FORMAT_R16G16_SNORM;
		//	case RenderFormat::R16G16_SINT:						return VK_FORMAT_R16G16_SINT;
		//	case RenderFormat::R32_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::D32_FLOAT:						return VK_FORMAT_D32_SFLOAT;
		//	case RenderFormat::R32_FLOAT:						return VK_FORMAT_R32_SFLOAT;
		//	case RenderFormat::R32_UINT:						return VK_FORMAT_R32_UINT;
		//	case RenderFormat::R32_SINT:						return VK_FORMAT_R32_SINT;
		//	case RenderFormat::R24G8_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::D24_UNORM_S8_UINT:				return VK_FORMAT_D24_UNORM_S8_UINT;
		//	case RenderFormat::R24_UNORM_X8_TYPELESS:			return VK_FORMAT_D24_UNORM_S8_UINT;
		//	case RenderFormat::X24_TYPELESS_G8_UINT:			return VK_FORMAT_D24_UNORM_S8_UINT;
		//	case RenderFormat::R8G8_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R8G8_UNORM:						return VK_FORMAT_R8G8_UNORM;
		//	case RenderFormat::R8G8_UINT:						return VK_FORMAT_R8G8_UINT;
		//	case RenderFormat::R8G8_SNORM:						return VK_FORMAT_R8G8_SNORM;
		//	case RenderFormat::R8G8_SINT:						return VK_FORMAT_R8G8_SINT;
		//	case RenderFormat::R16_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R16_FLOAT:						return VK_FORMAT_R16_SFLOAT;
		//	case RenderFormat::D16_UNORM:						return VK_FORMAT_D16_UNORM;
		//	case RenderFormat::R16_UNORM:						return VK_FORMAT_R16_UNORM;
		//	case RenderFormat::R16_UINT:						return VK_FORMAT_R16_UINT;
		//	case RenderFormat::R16_SNORM:						return VK_FORMAT_R16_SNORM;
		//	case RenderFormat::R16_SINT:						return VK_FORMAT_R16_SINT;
		//	case RenderFormat::R8_TYPELESS:						return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R8_UNORM:						return VK_FORMAT_R8_UNORM;
		//	case RenderFormat::R8_UINT:							return VK_FORMAT_R8_UINT;
		//	case RenderFormat::R8_SNORM:						return VK_FORMAT_R8_SNORM;
		//	case RenderFormat::R8_SINT:							return VK_FORMAT_R8_SINT;
		//	case RenderFormat::A8_UNORM:						return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R1_UNORM:						return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::R9G9B9E5_SHAREDEXP:				return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		//	case RenderFormat::R8G8_B8G8_UNORM:					return VK_FORMAT_B8G8R8G8_422_UNORM;
		//	case RenderFormat::G8R8_G8B8_UNORM:					return VK_FORMAT_G8B8G8R8_422_UNORM;
		//	case RenderFormat::BC1_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC1_UNORM:						return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		//	case RenderFormat::BC1_UNORM_SRGB:					return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		//	case RenderFormat::BC2_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC2_UNORM:						return VK_FORMAT_BC2_UNORM_BLOCK;
		//	case RenderFormat::BC2_UNORM_SRGB:					return VK_FORMAT_BC2_SRGB_BLOCK;
		//	case RenderFormat::BC3_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC3_UNORM:						return VK_FORMAT_BC3_UNORM_BLOCK;
		//	case RenderFormat::BC3_UNORM_SRGB:					return VK_FORMAT_BC3_SRGB_BLOCK;
		//	case RenderFormat::BC4_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC4_UNORM:						return VK_FORMAT_BC4_UNORM_BLOCK;
		//	case RenderFormat::BC4_SNORM:						return VK_FORMAT_BC4_SNORM_BLOCK;
		//	case RenderFormat::BC5_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC5_UNORM:						return VK_FORMAT_BC5_UNORM_BLOCK;
		//	case RenderFormat::BC5_SNORM:						return VK_FORMAT_BC5_SNORM_BLOCK;
		//	case RenderFormat::B5G6R5_UNORM:					return VK_FORMAT_B5G6R5_UNORM_PACK16;
		//	case RenderFormat::B5G5R5A1_UNORM:					return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
		//	case RenderFormat::B8G8R8A8_UNORM:					return VK_FORMAT_B8G8R8A8_UNORM;
		//	case RenderFormat::B8G8R8X8_UNORM:					return VK_FORMAT_B8G8R8A8_UNORM;
		//	case RenderFormat::R10G10B10_XR_BIAS_A2_UNORM:		return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::B8G8R8A8_TYPELESS:				return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::B8G8R8A8_UNORM_SRGB:				return VK_FORMAT_B8G8R8A8_SRGB;
		//	case RenderFormat::B8G8R8X8_TYPELESS:				return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::B8G8R8X8_UNORM_SRGB:				return VK_FORMAT_B8G8R8A8_SRGB;
		//	case RenderFormat::BC6H_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC6H_UF16:						return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		//	case RenderFormat::BC6H_SF16:						return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		//	case RenderFormat::BC7_TYPELESS:					return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::BC7_UNORM:						return VK_FORMAT_BC7_UNORM_BLOCK;
		//	case RenderFormat::BC7_UNORM_SRGB:					return VK_FORMAT_BC7_SRGB_BLOCK;
		//	case RenderFormat::AYUV:							return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
		//	case RenderFormat::Y410:							return VK_FORMAT_UNDEFINED; // HDR YUV 포맷, Vulkan 미지원
		//	case RenderFormat::Y416:							return VK_FORMAT_UNDEFINED; // HDR YUV 포맷, Vulkan 미지원
		//	case RenderFormat::NV12:							return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
		//	case RenderFormat::P010:							return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
		//	case RenderFormat::P016:							return VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
		//	case RenderFormat::YUY2:							return VK_FORMAT_G8B8G8R8_422_UNORM;
		//	case RenderFormat::Y210:							return VK_FORMAT_UNDEFINED; // 10-bit YUV 4:2:2 포맷, Vulkan 미지원
		//	case RenderFormat::Y216:							return VK_FORMAT_UNDEFINED; // 16-bit YUV 4:2:2 포맷, Vulkan 미지원
		//	case RenderFormat::NV11:							return VK_FORMAT_UNDEFINED; // 4:1:1 포맷, Vulkan 미지원
		//	case RenderFormat::AI44:							return VK_FORMAT_UNDEFINED; // Legacy format, Vulkan 미지원
		//	case RenderFormat::IA44:							return VK_FORMAT_UNDEFINED; // Legacy format, Vulkan 미지원
		//	case RenderFormat::P8:								return VK_FORMAT_UNDEFINED; // 8-bit palette, Vulkan 미지원
		//	case RenderFormat::A8P8:							return VK_FORMAT_UNDEFINED; // Legacy format, Vulkan 미지원
		//	case RenderFormat::B4G4R4A4_UNORM:					return VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT;
		//	case RenderFormat::P208:							return VK_FORMAT_UNDEFINED; // Planar 4:2:2, Vulkan 미지원
		//	case RenderFormat::V208:							return VK_FORMAT_UNDEFINED; // Vendor-specific, Vulkan 미지원
		//	case RenderFormat::V408:							return VK_FORMAT_UNDEFINED; // Vendor-specific, Vulkan 미지원
		//	case RenderFormat::SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: return VK_FORMAT_UNDEFINED;
		//	case RenderFormat::SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return VK_FORMAT_UNDEFINED; // Vulkan에선 Sampler Feedback 직접적으로 없음
		//	default: return VK_FORMAT_UNDEFINED;
		//	}
		//}







		RenderFormat GraphicsUtility::ConvertSPIRVTypeToRenderFormat(spirv_cross::SPIRType::BaseType _baseType, UInt32 _componentCount)
		{
			switch (_baseType)
			{
			case spirv_cross::SPIRType::Float:
				switch (_componentCount)
				{
				case 1: return RenderFormat::R32_FLOAT;
				case 2: return RenderFormat::R32G32_FLOAT;
				case 3: return RenderFormat::R32G32B32_FLOAT;
				case 4: return RenderFormat::R32G32B32A32_FLOAT;
				}
				break;
			case spirv_cross::SPIRType::Int:
				switch (_componentCount)
				{
				case 1: return RenderFormat::R32_SINT;
				case 2: return RenderFormat::R32G32_SINT;
				case 3: return RenderFormat::R32G32B32_SINT;
				case 4: return RenderFormat::R32G32B32A32_SINT;
				}
				break;
			case spirv_cross::SPIRType::UInt:
				switch (_componentCount)
				{
				case 1: return RenderFormat::R32_UINT;
				case 2: return RenderFormat::R32G32_UINT;
				case 3: return RenderFormat::R32G32B32_UINT;
				case 4: return RenderFormat::R32G32B32A32_UINT;
				}
				break;
			}
			return RenderFormat::UNKNOWN;
		}

		String GetShaderEntryPointName(ShaderType _type)
		{
			switch (_type)
			{
			case ShaderType::None:
			{
				DAYDREAM_CORE_ASSERT(false, "Invalid type");
				return nullptr;
			}
			case ShaderType::Vertex:	return "VSMain";
			case ShaderType::Hull:		return "HSMain";
			case ShaderType::Domain:	return "DSMain";
			case ShaderType::Geometry:	return "GSMain";
			case ShaderType::Pixel:		return "PSMain";
			case ShaderType::Compute:	return "CSMain";
			default:
				break;
			}
			DAYDREAM_CORE_ASSERT(false, "Invalid type");
			return nullptr;
		}

		WideString GraphicsUtility::GetShaderEntryPointNameW(ShaderType _type)
		{
			switch (_type)
			{
			case ShaderType::None:
			{
				DAYDREAM_CORE_ASSERT(false, "Invalid type");
				return nullptr;
			}
			case ShaderType::Vertex:	return L"VSMain";
			case ShaderType::Hull:		return L"HSMain";
			case ShaderType::Domain:	return L"DSMain";
			case ShaderType::Geometry:	return L"GSMain";
			case ShaderType::Pixel:		return L"PSMain";
			case ShaderType::Compute:	return L"CSMain";
			default:
				break;
			}
			DAYDREAM_CORE_ASSERT(false, "Invalid type");
			return nullptr;
		}

		String GetShaderTargetName(ShaderType _type, String _version)
		{
			switch (_type)
			{
			case ShaderType::None:
			{
				DAYDREAM_CORE_ASSERT(false, "Invalid type");
				return nullptr;
			}
			case ShaderType::Vertex:	return "vs_" + _version;
			case ShaderType::Hull:		return "hs_" + _version;
			case ShaderType::Domain:	return "ds_" + _version;
			case ShaderType::Geometry:	return "gs_" + _version;
			case ShaderType::Pixel:		return "ps_" + _version;
			case ShaderType::Compute:	return "cs_" + _version;
			default:
				break;
			}
			DAYDREAM_CORE_ASSERT(false, "Invalid type");
			return nullptr;
		}

		WideString GetShaderTargetNameW(ShaderType _type, WideString _version)
		{
			switch (_type)
			{
			case ShaderType::None:
			{
				DAYDREAM_CORE_ASSERT(false, "Invalid type");
				return nullptr;
			}
			case ShaderType::Vertex:	return L"vs_" + _version;
			case ShaderType::Hull:		return L"hs_" + _version;
			case ShaderType::Domain:	return L"ds_" + _version;
			case ShaderType::Geometry:	return L"gs_" + _version;
			case ShaderType::Pixel:		return L"ps_" + _version;
			case ShaderType::Compute:	return L"cs_" + _version;
			default:
				break;
			}
			DAYDREAM_CORE_ASSERT(false, "Invalid type");
			return nullptr;
		}

		String GetVendor(UInt32 _vendorCode)
		{
			switch (_vendorCode)
			{
			case 0x10DE: return "NVIDIA Corporation";
			case 0x1002: return "AMD Inc.";
			case 0x8086: return "Intel";
			case 0x1414: return "Microsoft";
			}
			DAYDREAM_CORE_ERROR("Not a valid VendorID");
			return "";
		}

		String GetAPIString(RendererAPIType _API)
		{
			switch (_API)
			{
			case RendererAPIType::OpenGL:
				return "GL";
			case RendererAPIType::DirectX11:
				return "DX11";
			case RendererAPIType::DirectX12:
				return "DX12";
			case RendererAPIType::Vulkan:
				return "VK";
			case RendererAPIType::Metal:
				return "MTL";
			default:
				return "";
			}
		}

		bool IsDepthFormat(RenderFormat _format)
		{
			switch (_format)
			{
			case RenderFormat::D24_UNORM_S8_UINT:
			case RenderFormat::D32_FLOAT:
			case RenderFormat::D16_UNORM:
			case RenderFormat::D32_FLOAT_S8X24_UINT:

			case RenderFormat::R24G8_TYPELESS:
			case RenderFormat::R32_TYPELESS:
			case RenderFormat::R16_TYPELESS:
			case RenderFormat::R32G8X24_TYPELESS:
			case RenderFormat::R24_UNORM_X8_TYPELESS:
			case RenderFormat::R32_FLOAT_X8X24_TYPELESS:
				return true;
			default:
				return false;
			}
		}

		bool IsStencilFormat(RenderFormat _format)
		{
			switch (_format)
			{
			case RenderFormat::D24_UNORM_S8_UINT:
			case RenderFormat::D32_FLOAT_S8X24_UINT:

			case RenderFormat::R24G8_TYPELESS:
			case RenderFormat::R32G8X24_TYPELESS:
			case RenderFormat::X24_TYPELESS_G8_UINT:
			case RenderFormat::X32_TYPELESS_G8X24_UINT:
				return true;
			default:
				return false;
			}
		}


	}
}