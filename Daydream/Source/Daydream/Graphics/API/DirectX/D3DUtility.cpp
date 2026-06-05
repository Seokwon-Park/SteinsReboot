#include "DaydreamPCH.h"
#include "D3DUtility.h"

namespace Daydream::GraphicsUtility::DirectX
{
	DXGI_FORMAT ConvertToDXGIFormat(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:      return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Mat3x3:     return DXGI_FORMAT_UNKNOWN;
		case ShaderDataType::Mat4x4:     return DXGI_FORMAT_UNKNOWN;
		case ShaderDataType::Bool:     return DXGI_FORMAT_UNKNOWN;
		}

		DAYDREAM_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT ConvertToDXGIFormat(RenderFormat _format)
	{
		switch (_format)
		{
		case RenderFormat::UNKNOWN:                             return DXGI_FORMAT_UNKNOWN;
		case RenderFormat::R32G32B32A32_TYPELESS:               return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case RenderFormat::R32G32B32A32_FLOAT:                  return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case RenderFormat::R32G32B32A32_UINT:                   return DXGI_FORMAT_R32G32B32A32_UINT;
		case RenderFormat::R32G32B32A32_SINT:                   return DXGI_FORMAT_R32G32B32A32_SINT;
		case RenderFormat::R32G32B32_TYPELESS:                  return DXGI_FORMAT_R32G32B32_TYPELESS;
		case RenderFormat::R32G32B32_FLOAT:                     return DXGI_FORMAT_R32G32B32_FLOAT;
		case RenderFormat::R32G32B32_UINT:                      return DXGI_FORMAT_R32G32B32_UINT;
		case RenderFormat::R32G32B32_SINT:                      return DXGI_FORMAT_R32G32B32_SINT;
		case RenderFormat::R16G16B16A16_TYPELESS:               return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case RenderFormat::R16G16B16A16_FLOAT:                  return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case RenderFormat::R16G16B16A16_UNORM:                  return DXGI_FORMAT_R16G16B16A16_UNORM;
		case RenderFormat::R16G16B16A16_UINT:                   return DXGI_FORMAT_R16G16B16A16_UINT;
		case RenderFormat::R16G16B16A16_SNORM:                  return DXGI_FORMAT_R16G16B16A16_SNORM;
		case RenderFormat::R16G16B16A16_SINT:                   return DXGI_FORMAT_R16G16B16A16_SINT;
		case RenderFormat::R32G32_TYPELESS:                     return DXGI_FORMAT_R32G32_TYPELESS;
		case RenderFormat::R32G32_FLOAT:                        return DXGI_FORMAT_R32G32_FLOAT;
		case RenderFormat::R32G32_UINT:                         return DXGI_FORMAT_R32G32_UINT;
		case RenderFormat::R32G32_SINT:                         return DXGI_FORMAT_R32G32_SINT;
		case RenderFormat::R32G8X24_TYPELESS:                   return DXGI_FORMAT_R32G8X24_TYPELESS;
		case RenderFormat::D32_FLOAT_S8X24_UINT:                return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case RenderFormat::R32_FLOAT_X8X24_TYPELESS:            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case RenderFormat::X32_TYPELESS_G8X24_UINT:             return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		case RenderFormat::R10G10B10A2_TYPELESS:                return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		case RenderFormat::R10G10B10A2_UNORM:                   return DXGI_FORMAT_R10G10B10A2_UNORM;
		case RenderFormat::R10G10B10A2_UINT:                    return DXGI_FORMAT_R10G10B10A2_UINT;
		case RenderFormat::R11G11B10_FLOAT:                     return DXGI_FORMAT_R11G11B10_FLOAT;
		case RenderFormat::R8G8B8A8_TYPELESS:                   return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case RenderFormat::R8G8B8A8_UNORM:                      return DXGI_FORMAT_R8G8B8A8_UNORM;
		case RenderFormat::R8G8B8A8_UNORM_SRGB:                 return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case RenderFormat::R8G8B8A8_UINT:                       return DXGI_FORMAT_R8G8B8A8_UINT;
		case RenderFormat::R8G8B8A8_SNORM:                      return DXGI_FORMAT_R8G8B8A8_SNORM;
		case RenderFormat::R8G8B8A8_SINT:                       return DXGI_FORMAT_R8G8B8A8_SINT;
		case RenderFormat::R16G16_TYPELESS:                     return DXGI_FORMAT_R16G16_TYPELESS;
		case RenderFormat::R16G16_FLOAT:                        return DXGI_FORMAT_R16G16_FLOAT;
		case RenderFormat::R16G16_UNORM:                        return DXGI_FORMAT_R16G16_UNORM;
		case RenderFormat::R16G16_UINT:                         return DXGI_FORMAT_R16G16_UINT;
		case RenderFormat::R16G16_SNORM:                        return DXGI_FORMAT_R16G16_SNORM;
		case RenderFormat::R16G16_SINT:                         return DXGI_FORMAT_R16G16_SINT;
		case RenderFormat::R32_TYPELESS:                        return DXGI_FORMAT_R32_TYPELESS;
		case RenderFormat::D32_FLOAT:                           return DXGI_FORMAT_D32_FLOAT;
		case RenderFormat::R32_FLOAT:                           return DXGI_FORMAT_R32_FLOAT;
		case RenderFormat::R32_UINT:                            return DXGI_FORMAT_R32_UINT;
		case RenderFormat::R32_SINT:                            return DXGI_FORMAT_R32_SINT;
		case RenderFormat::R24G8_TYPELESS:                      return DXGI_FORMAT_R24G8_TYPELESS;
		case RenderFormat::D24_UNORM_S8_UINT:                   return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case RenderFormat::R24_UNORM_X8_TYPELESS:               return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case RenderFormat::X24_TYPELESS_G8_UINT:                return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		case RenderFormat::R8G8_TYPELESS:                       return DXGI_FORMAT_R8G8_TYPELESS;
		case RenderFormat::R8G8_UNORM:                          return DXGI_FORMAT_R8G8_UNORM;
		case RenderFormat::R8G8_UINT:                           return DXGI_FORMAT_R8G8_UINT;
		case RenderFormat::R8G8_SNORM:                          return DXGI_FORMAT_R8G8_SNORM;
		case RenderFormat::R8G8_SINT:                           return DXGI_FORMAT_R8G8_SINT;
		case RenderFormat::R16_TYPELESS:                        return DXGI_FORMAT_R16_TYPELESS;
		case RenderFormat::R16_FLOAT:                           return DXGI_FORMAT_R16_FLOAT;
		case RenderFormat::D16_UNORM:                           return DXGI_FORMAT_D16_UNORM;
		case RenderFormat::R16_UNORM:                           return DXGI_FORMAT_R16_UNORM;
		case RenderFormat::R16_UINT:                            return DXGI_FORMAT_R16_UINT;
		case RenderFormat::R16_SNORM:                           return DXGI_FORMAT_R16_SNORM;
		case RenderFormat::R16_SINT:                            return DXGI_FORMAT_R16_SINT;
		case RenderFormat::R8_TYPELESS:                         return DXGI_FORMAT_R8_TYPELESS;
		case RenderFormat::R8_UNORM:                            return DXGI_FORMAT_R8_UNORM;
		case RenderFormat::R8_UINT:                             return DXGI_FORMAT_R8_UINT;
		case RenderFormat::R8_SNORM:                            return DXGI_FORMAT_R8_SNORM;
		case RenderFormat::R8_SINT:                             return DXGI_FORMAT_R8_SINT;
		case RenderFormat::A8_UNORM:                            return DXGI_FORMAT_A8_UNORM;
		case RenderFormat::R1_UNORM:                            return DXGI_FORMAT_R1_UNORM;
		case RenderFormat::R9G9B9E5_SHAREDEXP:                  return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case RenderFormat::R8G8_B8G8_UNORM:                     return DXGI_FORMAT_R8G8_B8G8_UNORM;
		case RenderFormat::G8R8_G8B8_UNORM:                     return DXGI_FORMAT_G8R8_G8B8_UNORM;
		case RenderFormat::BC1_TYPELESS:                        return DXGI_FORMAT_BC1_TYPELESS;
		case RenderFormat::BC1_UNORM:                           return DXGI_FORMAT_BC1_UNORM;
		case RenderFormat::BC1_UNORM_SRGB:                      return DXGI_FORMAT_BC1_UNORM_SRGB;
		case RenderFormat::BC2_TYPELESS:                        return DXGI_FORMAT_BC2_TYPELESS;
		case RenderFormat::BC2_UNORM:                           return DXGI_FORMAT_BC2_UNORM;
		case RenderFormat::BC2_UNORM_SRGB:                      return DXGI_FORMAT_BC2_UNORM_SRGB;
		case RenderFormat::BC3_TYPELESS:                        return DXGI_FORMAT_BC3_TYPELESS;
		case RenderFormat::BC3_UNORM:                           return DXGI_FORMAT_BC3_UNORM;
		case RenderFormat::BC3_UNORM_SRGB:                      return DXGI_FORMAT_BC3_UNORM_SRGB;
		case RenderFormat::BC4_TYPELESS:                        return DXGI_FORMAT_BC4_TYPELESS;
		case RenderFormat::BC4_UNORM:                           return DXGI_FORMAT_BC4_UNORM;
		case RenderFormat::BC4_SNORM:                           return DXGI_FORMAT_BC4_SNORM;
		case RenderFormat::BC5_TYPELESS:                        return DXGI_FORMAT_BC5_TYPELESS;
		case RenderFormat::BC5_UNORM:                           return DXGI_FORMAT_BC5_UNORM;
		case RenderFormat::BC5_SNORM:                           return DXGI_FORMAT_BC5_SNORM;
		case RenderFormat::B5G6R5_UNORM:                        return DXGI_FORMAT_B5G6R5_UNORM;
		case RenderFormat::B5G5R5A1_UNORM:                      return DXGI_FORMAT_B5G5R5A1_UNORM;
		case RenderFormat::B8G8R8A8_UNORM:                      return DXGI_FORMAT_B8G8R8A8_UNORM;
		case RenderFormat::B8G8R8X8_UNORM:                      return DXGI_FORMAT_B8G8R8X8_UNORM;
		case RenderFormat::R10G10B10_XR_BIAS_A2_UNORM:          return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		case RenderFormat::B8G8R8A8_TYPELESS:                   return DXGI_FORMAT_B8G8R8A8_TYPELESS;
		case RenderFormat::B8G8R8A8_UNORM_SRGB:                 return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case RenderFormat::B8G8R8X8_TYPELESS:                   return DXGI_FORMAT_B8G8R8X8_TYPELESS;
		case RenderFormat::B8G8R8X8_UNORM_SRGB:                 return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		case RenderFormat::BC6H_TYPELESS:                       return DXGI_FORMAT_BC6H_TYPELESS;
		case RenderFormat::BC6H_UF16:                           return DXGI_FORMAT_BC6H_UF16;
		case RenderFormat::BC6H_SF16:                           return DXGI_FORMAT_BC6H_SF16;
		case RenderFormat::BC7_TYPELESS:                        return DXGI_FORMAT_BC7_TYPELESS;
		case RenderFormat::BC7_UNORM:                           return DXGI_FORMAT_BC7_UNORM;
		case RenderFormat::BC7_UNORM_SRGB:                      return DXGI_FORMAT_BC7_UNORM_SRGB;
		case RenderFormat::AYUV:                                return DXGI_FORMAT_AYUV;
		case RenderFormat::Y410:                                return DXGI_FORMAT_Y410;
		case RenderFormat::Y416:                                return DXGI_FORMAT_Y416;
		case RenderFormat::NV12:                                return DXGI_FORMAT_NV12;
		case RenderFormat::P010:                                return DXGI_FORMAT_P010;
		case RenderFormat::P016:                                return DXGI_FORMAT_P016;
		case RenderFormat::YUY2:                                return DXGI_FORMAT_YUY2;
		case RenderFormat::Y210:                                return DXGI_FORMAT_Y210;
		case RenderFormat::Y216:                                return DXGI_FORMAT_Y216;
		case RenderFormat::NV11:                                return DXGI_FORMAT_NV11;
		case RenderFormat::AI44:                                return DXGI_FORMAT_AI44;
		case RenderFormat::IA44:                                return DXGI_FORMAT_IA44;
		case RenderFormat::P8:                                  return DXGI_FORMAT_P8;
		case RenderFormat::A8P8:                                return DXGI_FORMAT_A8P8;
		case RenderFormat::B4G4R4A4_UNORM:                      return DXGI_FORMAT_B4G4R4A4_UNORM;
		case RenderFormat::P208:                                return DXGI_FORMAT_P208;
		case RenderFormat::V208:                                return DXGI_FORMAT_V208;
		case RenderFormat::V408:                                return DXGI_FORMAT_V408;
		case RenderFormat::SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:    return DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
		case RenderFormat::SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
		default:                                                        return DXGI_FORMAT_UNKNOWN;
		}
	}
}