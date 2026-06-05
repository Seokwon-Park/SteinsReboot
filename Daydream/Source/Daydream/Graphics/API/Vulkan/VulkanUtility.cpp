#include "DaydreamPCH.h"
#include "VulkanUtility.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream::GraphicsUtility::Vulkan
{
	Pair<vk::PipelineStageFlags, vk::AccessFlags> ConvertToVulkanStageAndAccess(ResourceState _state)
	{
		vk::PipelineStageFlags stage;
		vk::AccessFlags access;

		switch (_state)
		{
		case ResourceState::Undefined:
			stage = vk::PipelineStageFlagBits::eTopOfPipe;
			access = vk::AccessFlags(0);
			break;
		case ResourceState::ShaderResource:
			stage = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
			access = vk::AccessFlagBits::eShaderRead;
			break;
		case ResourceState::CopySource:
			stage = vk::PipelineStageFlagBits::eTransfer;
			access = vk::AccessFlagBits::eTransferRead;
			break;
		case ResourceState::CopyDest:
			stage = vk::PipelineStageFlagBits::eTransfer;
			access = vk::AccessFlagBits::eTransferWrite;
			break;
		case ResourceState::RenderTarget:
			stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			access = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
			break;
		case ResourceState::DepthRead:
			stage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
			access = vk::AccessFlagBits::eDepthStencilAttachmentRead;
			break;
		case ResourceState::DepthWrite:
			stage = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
			access = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			break;
		case ResourceState::Present:
			stage = vk::PipelineStageFlagBits::eBottomOfPipe;
			access = vk::AccessFlags(0);
			break;
		case ResourceState::VertexBuffer:
			stage = vk::PipelineStageFlagBits::eVertexInput;
			access = vk::AccessFlagBits::eVertexAttributeRead;
			break;
		case ResourceState::IndexBuffer:
			stage = vk::PipelineStageFlagBits::eVertexInput;
			access = vk::AccessFlagBits::eIndexRead;
			break;
		case ResourceState::ConstantBuffer:
			stage = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
			access = vk::AccessFlagBits::eUniformRead;
			break;
		case ResourceState::UnorderedAccess:
			stage = vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eFragmentShader;
			access = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
			break;
		default:
			stage = vk::PipelineStageFlagBits::eBottomOfPipe;
			access = vk::AccessFlags(0);
			break;
		}

		return { stage, access };
	}

	vk::ImageLayout ConvertToVulkanImageLayout(ResourceState _state)
	{
		switch (_state)
		{
		case ResourceState::Undefined:
			return vk::ImageLayout::eUndefined;
		case ResourceState::ShaderResource:
			return vk::ImageLayout::eShaderReadOnlyOptimal;
		case ResourceState::CopySource:
			return vk::ImageLayout::eTransferSrcOptimal;
		case ResourceState::CopyDest:
			return vk::ImageLayout::eTransferDstOptimal;
		case ResourceState::RenderTarget:
			return vk::ImageLayout::eColorAttachmentOptimal;
		case ResourceState::DepthRead:
			return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
		case ResourceState::DepthWrite:
			return vk::ImageLayout::eDepthStencilAttachmentOptimal;
		case ResourceState::UnorderedAccess:
			return vk::ImageLayout::eGeneral;
		case ResourceState::Present:
			return vk::ImageLayout::ePresentSrcKHR;
		case ResourceState::VertexBuffer:
		case ResourceState::IndexBuffer:
		case ResourceState::ConstantBuffer:
			return vk::ImageLayout::eUndefined;
		default:
			return vk::ImageLayout::eUndefined;
		}
	}

	vk::ImageAspectFlags GetImageAspectFlags(RenderFormat _format)
	{
		switch (_format)
		{
		case RenderFormat::R32_TYPELESS:
		case RenderFormat::R16_TYPELESS:
		case RenderFormat::D16_UNORM:
		case RenderFormat::D32_FLOAT:
			return vk::ImageAspectFlagBits::eDepth;


		case RenderFormat::R24G8_TYPELESS:
		case RenderFormat::R32G8X24_TYPELESS:
		case RenderFormat::R24_UNORM_X8_TYPELESS:
		case RenderFormat::R32_FLOAT_X8X24_TYPELESS:
		case RenderFormat::D32_FLOAT_S8X24_UINT:
		case RenderFormat::D24_UNORM_S8_UINT:
			return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

		default:
			return vk::ImageAspectFlagBits::eColor;
		}
	}

	vk::BufferCreateInfo ConvertToVkImageCreateInfo(const BufferDesc& _desc)
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = _desc.size;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		// 비트 플래그 매핑 (Bitwise Mapping)
		vk::BufferUsageFlags flags;

		if (HasFlag(_desc.bufferUsage, BufferUsage::Vertex)) flags |= vk::BufferUsageFlagBits::eVertexBuffer;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Index)) flags |= vk::BufferUsageFlagBits::eIndexBuffer;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Constant)) flags |= vk::BufferUsageFlagBits::eUniformBuffer;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Storage)) flags |= vk::BufferUsageFlagBits::eStorageBuffer;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Indirect)) flags |= vk::BufferUsageFlagBits::eIndirectBuffer;

		switch (_desc.memoryUsage)
		{
		case MemoryUsage::Static:
			flags |= vk::BufferUsageFlagBits::eTransferDst;
			flags |= vk::BufferUsageFlagBits::eTransferSrc;
			break;
		case MemoryUsage::Dynamic:
			break;
		case MemoryUsage::Upload:
			flags = vk::BufferUsageFlagBits::eTransferSrc;
			break;
		case MemoryUsage::Readback:
			flags |= vk::BufferUsageFlagBits::eTransferDst;
			break;
		default:
			break;
		}

		bufferInfo.usage = flags;
		return bufferInfo;
	}

	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const BufferDesc& _desc)
	{
		vma::AllocationCreateInfo allocInfo{};

		switch (_desc.memoryUsage)
		{
		case MemoryUsage::Static:
			// GPU 전용 (가장 빠름)
			allocInfo.usage = vma::MemoryUsage::eGpuOnly;
			break;

		case MemoryUsage::Dynamic:
			// 매 프레임 CPU가 쓰는 용도
			allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
			allocInfo.flags = vma::AllocationCreateFlagBits::eMapped | vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
			break;
		case MemoryUsage::Readback:
			// CPU가 GPU 결과값을 읽어오는 용도 (엔티티 피킹 등)
			allocInfo.usage = vma::MemoryUsage::eGpuToCpu;
			allocInfo.flags = vma::AllocationCreateFlagBits::eMapped | vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
			break;
		case MemoryUsage::Upload:
			allocInfo.usage = vma::MemoryUsage::eCpuOnly;
			allocInfo.flags = vma::AllocationCreateFlagBits::eMapped | vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
			break;
		}

		return allocInfo;
	}

	vk::ImageCreateInfo ConvertToVulkanCreateInfo(const TextureDesc& _desc)
	{
		vk::ImageCreateInfo imageInfo{};

		switch (_desc.type)
		{
		case TextureType::Texture1D:
		case TextureType::Texture1DArray:
		{
			imageInfo.imageType = vk::ImageType::e1D;
			imageInfo.extent.width = _desc.width;
			imageInfo.extent.height = 1;
			imageInfo.extent.depth = 1;
			imageInfo.arrayLayers = (_desc.type == TextureType::Texture1DArray) ? _desc.layerCount : 1;
			break;
		}
		case TextureType::Texture2D:
		case TextureType::Texture2DArray:
		case TextureType::Texture2DMultisample:
		{
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = _desc.width;
			imageInfo.extent.height = _desc.height;
			imageInfo.extent.depth = 1;
			imageInfo.arrayLayers = (_desc.type == TextureType::Texture2DArray) ? _desc.layerCount : 1;
			if (_desc.type == TextureType::Texture2DMultisample)
			{
				imageInfo.mipLevels = 1;
			}
			break;
		}

		case TextureType::TextureCube:
		case TextureType::TextureCubeArray:
		{
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = _desc.width;
			imageInfo.extent.height = _desc.height;
			imageInfo.extent.depth = 1;
			imageInfo.arrayLayers = (_desc.type == TextureType::TextureCubeArray) ? (6 * _desc.layerCount) : 6;
			imageInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;
			break;
		}

		case TextureType::Texture3D:
		{
			imageInfo.imageType = vk::ImageType::e3D;
			imageInfo.extent.width = _desc.width;
			imageInfo.extent.height = _desc.height;
			imageInfo.extent.depth = _desc.layerCount;
			imageInfo.arrayLayers = 1;
			break;
		}

		case TextureType::Unknown:
		default:
			// DAYDREAM_CORE_ASSERT(false, "Unknown Texture Type in Vulkan conversion!");
			break;
		}

		imageInfo.mipLevels = _desc.mipLevels;
		imageInfo.arrayLayers = _desc.layerCount;
		imageInfo.format = ConvertToVkFormat(_desc.format);
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = ConvertToVkImageUsageFlags(_desc.textureUsage);
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.samples = Cast<vk::SampleCountFlagBits>(_desc.sampleCount);

		return imageInfo;
	}

	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const TextureDesc& _desc)
	{
		vma::AllocationCreateInfo allocInfo{};

		allocInfo.usage = vma::MemoryUsage::eAuto;
		if ((UInt32)_desc.textureUsage & (UInt32)TextureUsage::RenderTarget)
		{
			allocInfo.flags |= vma::AllocationCreateFlagBits::eDedicatedMemory;
		}

		return allocInfo;
	}

	vk::AttachmentLoadOp ConvertToLoadOp(AttachmentLoadOp op)
	{
		switch (op)
		{
		case AttachmentLoadOp::Load:     return vk::AttachmentLoadOp::eLoad;
		case AttachmentLoadOp::Clear:    return vk::AttachmentLoadOp::eClear;
		case AttachmentLoadOp::DontCare: return vk::AttachmentLoadOp::eDontCare;
		default:                         return vk::AttachmentLoadOp::eDontCare;
		}
	}

	vk::AttachmentStoreOp ConvertToStoreOp(AttachmentStoreOp op)
	{
		switch (op)
		{
		case AttachmentStoreOp::Store:    return vk::AttachmentStoreOp::eStore;
		case AttachmentStoreOp::DontCare: return vk::AttachmentStoreOp::eDontCare;
		default:                          return vk::AttachmentStoreOp::eDontCare;
		}
	}


	vk::Format ConvertToVkFormat(RenderFormat _format)
	{
		switch (_format)
		{
		case RenderFormat::UNKNOWN:							return vk::Format::eUndefined;
		case RenderFormat::R32G32B32A32_TYPELESS:			return vk::Format::eUndefined;
		case RenderFormat::R32G32B32A32_FLOAT:				return vk::Format::eR32G32B32A32Sfloat;
		case RenderFormat::R32G32B32A32_UINT:				return vk::Format::eR32G32B32A32Uint;
		case RenderFormat::R32G32B32A32_SINT:				return vk::Format::eR32G32B32A32Sint;
		case RenderFormat::R32G32B32_TYPELESS:				return vk::Format::eUndefined;
		case RenderFormat::R32G32B32_FLOAT:					return vk::Format::eR32G32B32Sfloat;
		case RenderFormat::R32G32B32_UINT:					return vk::Format::eR32G32B32Uint;
		case RenderFormat::R32G32B32_SINT:					return vk::Format::eR32G32B32Sint;
		case RenderFormat::R16G16B16A16_TYPELESS:			return vk::Format::eUndefined;
		case RenderFormat::R16G16B16A16_FLOAT:				return vk::Format::eR16G16B16A16Sfloat;
		case RenderFormat::R16G16B16A16_UNORM:				return vk::Format::eR16G16B16A16Unorm;
		case RenderFormat::R16G16B16A16_UINT:				return vk::Format::eR16G16B16A16Uint;
		case RenderFormat::R16G16B16A16_SNORM:				return vk::Format::eR16G16B16A16Snorm;
		case RenderFormat::R16G16B16A16_SINT:				return vk::Format::eR16G16B16A16Sint;
		case RenderFormat::R32G32_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::R32G32_FLOAT:					return vk::Format::eR32G32Sfloat;
		case RenderFormat::R32G32_UINT:						return vk::Format::eR32G32Uint;
		case RenderFormat::R32G32_SINT:						return vk::Format::eR32G32Sint;
		case RenderFormat::R32G8X24_TYPELESS:				return vk::Format::eUndefined;
		case RenderFormat::D32_FLOAT_S8X24_UINT:			return vk::Format::eD32SfloatS8Uint;
		case RenderFormat::R32_FLOAT_X8X24_TYPELESS:		return vk::Format::eD32SfloatS8Uint;
		case RenderFormat::X32_TYPELESS_G8X24_UINT:			return vk::Format::eD32SfloatS8Uint;
		case RenderFormat::R10G10B10A2_TYPELESS:			return vk::Format::eUndefined;
		case RenderFormat::R10G10B10A2_UNORM:				return vk::Format::eA2B10G10R10UnormPack32;
		case RenderFormat::R10G10B10A2_UINT:				return vk::Format::eA2B10G10R10UintPack32;
		case RenderFormat::R11G11B10_FLOAT:					return vk::Format::eB10G11R11UfloatPack32;
		case RenderFormat::R8G8B8A8_TYPELESS:				return vk::Format::eUndefined;
		case RenderFormat::R8G8B8A8_UNORM:					return vk::Format::eR8G8B8A8Unorm;
		case RenderFormat::R8G8B8A8_UNORM_SRGB:				return vk::Format::eR8G8B8A8Srgb;
		case RenderFormat::R8G8B8A8_UINT:					return vk::Format::eR8G8B8A8Uint;
		case RenderFormat::R8G8B8A8_SNORM:					return vk::Format::eR8G8B8A8Snorm;
		case RenderFormat::R8G8B8A8_SINT:					return vk::Format::eR8G8B8A8Sint;
		case RenderFormat::R16G16_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::R16G16_FLOAT:					return vk::Format::eR16G16Sfloat;
		case RenderFormat::R16G16_UNORM:					return vk::Format::eR16G16Unorm;
		case RenderFormat::R16G16_UINT:						return vk::Format::eR16G16Uint;
		case RenderFormat::R16G16_SNORM:					return vk::Format::eR16G16Snorm;
		case RenderFormat::R16G16_SINT:						return vk::Format::eR16G16Sint;
		case RenderFormat::R32_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::D32_FLOAT:						return vk::Format::eD32Sfloat;
		case RenderFormat::R32_FLOAT:						return vk::Format::eR32Sfloat;
		case RenderFormat::R32_UINT:						return vk::Format::eR32Uint;
		case RenderFormat::R32_SINT:						return vk::Format::eR32Sint;
		case RenderFormat::R24G8_TYPELESS:					return vk::Format::eD24UnormS8Uint;
		case RenderFormat::D24_UNORM_S8_UINT:				return vk::Format::eD24UnormS8Uint;
		case RenderFormat::R24_UNORM_X8_TYPELESS:			return vk::Format::eD24UnormS8Uint;
		case RenderFormat::X24_TYPELESS_G8_UINT:			return vk::Format::eD24UnormS8Uint;
		case RenderFormat::R8G8_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::R8G8_UNORM:						return vk::Format::eR8G8Unorm;
		case RenderFormat::R8G8_UINT:						return vk::Format::eR8G8Uint;
		case RenderFormat::R8G8_SNORM:						return vk::Format::eR8G8Snorm;
		case RenderFormat::R8G8_SINT:						return vk::Format::eR8G8Sint;
		case RenderFormat::R16_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::R16_FLOAT:						return vk::Format::eR16Sfloat;
		case RenderFormat::D16_UNORM:						return vk::Format::eD16Unorm;
		case RenderFormat::R16_UNORM:						return vk::Format::eR16Unorm;
		case RenderFormat::R16_UINT:						return vk::Format::eR16Uint;
		case RenderFormat::R16_SNORM:						return vk::Format::eR16Snorm;
		case RenderFormat::R16_SINT:						return vk::Format::eR16Sint;
		case RenderFormat::R8_TYPELESS:						return vk::Format::eUndefined;
		case RenderFormat::R8_UNORM:						return vk::Format::eR8Unorm;
		case RenderFormat::R8_UINT:							return vk::Format::eR8Uint;
		case RenderFormat::R8_SNORM:						return vk::Format::eR8Snorm;
		case RenderFormat::R8_SINT:							return vk::Format::eR8Sint;
		case RenderFormat::A8_UNORM:						return vk::Format::eUndefined;
		case RenderFormat::R1_UNORM:						return vk::Format::eUndefined;
		case RenderFormat::R9G9B9E5_SHAREDEXP:				return vk::Format::eE5B9G9R9UfloatPack32;
		case RenderFormat::R8G8_B8G8_UNORM:					return vk::Format::eB8G8R8G8422Unorm;
		case RenderFormat::G8R8_G8B8_UNORM:					return vk::Format::eG8B8G8R8422Unorm;
		case RenderFormat::BC1_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC1_UNORM:						return vk::Format::eBc1RgbaUnormBlock;
		case RenderFormat::BC1_UNORM_SRGB:					return vk::Format::eBc1RgbaSrgbBlock;
		case RenderFormat::BC2_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC2_UNORM:						return vk::Format::eBc2UnormBlock;
		case RenderFormat::BC2_UNORM_SRGB:					return vk::Format::eBc2SrgbBlock;
		case RenderFormat::BC3_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC3_UNORM:						return vk::Format::eBc3UnormBlock;
		case RenderFormat::BC3_UNORM_SRGB:					return vk::Format::eBc3SrgbBlock;
		case RenderFormat::BC4_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC4_UNORM:						return vk::Format::eBc4UnormBlock;
		case RenderFormat::BC4_SNORM:						return vk::Format::eBc4SnormBlock;
		case RenderFormat::BC5_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC5_UNORM:						return vk::Format::eBc5UnormBlock;
		case RenderFormat::BC5_SNORM:						return vk::Format::eBc5SnormBlock;
		case RenderFormat::B5G6R5_UNORM:					return vk::Format::eB5G6R5UnormPack16;
		case RenderFormat::B5G5R5A1_UNORM:					return vk::Format::eB5G5R5A1UnormPack16;
		case RenderFormat::B8G8R8A8_UNORM:					return vk::Format::eB8G8R8A8Unorm;
		case RenderFormat::B8G8R8X8_UNORM:					return vk::Format::eB8G8R8A8Unorm;
			// Note: R10G10B10_XR_BIAS_A2_UNORM is a DirectX-specific format. No direct Vulkan equivalent.
		case RenderFormat::R10G10B10_XR_BIAS_A2_UNORM:		return vk::Format::eUndefined;
		case RenderFormat::B8G8R8A8_TYPELESS:				return vk::Format::eUndefined;
		case RenderFormat::B8G8R8A8_UNORM_SRGB:				return vk::Format::eB8G8R8A8Srgb;
		case RenderFormat::B8G8R8X8_TYPELESS:				return vk::Format::eUndefined;
		case RenderFormat::B8G8R8X8_UNORM_SRGB:				return vk::Format::eB8G8R8A8Srgb;
		case RenderFormat::BC6H_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC6H_UF16:						return vk::Format::eBc6HUfloatBlock;
		case RenderFormat::BC6H_SF16:						return vk::Format::eBc6HSfloatBlock;
		case RenderFormat::BC7_TYPELESS:					return vk::Format::eUndefined;
		case RenderFormat::BC7_UNORM:						return vk::Format::eBc7UnormBlock;
		case RenderFormat::BC7_UNORM_SRGB:					return vk::Format::eBc7SrgbBlock;
			// Note: AYUV is complex. This mapping might depend on specific usage.
		case RenderFormat::AYUV:							return vk::Format::eG8B8R83Plane420Unorm;
		case RenderFormat::Y410:							return vk::Format::eUndefined;
		case RenderFormat::Y416:							return vk::Format::eUndefined;
		case RenderFormat::NV12:							return vk::Format::eG8B8R82Plane420Unorm;
		case RenderFormat::P010:							return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16;
		case RenderFormat::P016:							return vk::Format::eG16B16R162Plane420Unorm;
		case RenderFormat::YUY2:							return vk::Format::eG8B8G8R8422Unorm;
		case RenderFormat::Y210:							return vk::Format::eUndefined;
		case RenderFormat::Y216:							return vk::Format::eUndefined;
		case RenderFormat::NV11:							return vk::Format::eUndefined;
		case RenderFormat::AI44:							return vk::Format::eUndefined;
		case RenderFormat::IA44:							return vk::Format::eUndefined;
		case RenderFormat::P8:								return vk::Format::eUndefined;
		case RenderFormat::A8P8:							return vk::Format::eUndefined;
		case RenderFormat::B4G4R4A4_UNORM:					return vk::Format::eA4R4G4B4UnormPack16EXT;
		case RenderFormat::P208:							return vk::Format::eUndefined;
		case RenderFormat::V208:							return vk::Format::eUndefined;
		case RenderFormat::V408:							return vk::Format::eUndefined;
		case RenderFormat::SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: return vk::Format::eUndefined;
		case RenderFormat::SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return vk::Format::eUndefined;
		default:
			// It's good practice to assert or log an error for unhandled cases.
			// assert(false && "Unknown RenderFormat!");
			return vk::Format::eUndefined;
		}
	}

	vk::ShaderStageFlagBits ConvertToShaderStageFlagBit(ShaderType _type)
	{
		switch (_type)
		{
		case ShaderType::Vertex:
			return vk::ShaderStageFlagBits::eVertex;
			break;
		case ShaderType::Hull:
			return vk::ShaderStageFlagBits::eTessellationControl;
			break;
		case ShaderType::Domain:
			return vk::ShaderStageFlagBits::eTessellationEvaluation;
			break;
		case ShaderType::Geometry:
			return vk::ShaderStageFlagBits::eGeometry;
			break;
		case ShaderType::Pixel:
			return vk::ShaderStageFlagBits::eFragment;
			break;
		case ShaderType::Compute:
			return vk::ShaderStageFlagBits::eCompute;
			break;
		}
		return vk::ShaderStageFlagBits::eAll;
	};

	vk::ImageUsageFlags ConvertToVkImageUsageFlags(TextureUsage _flags)
	{
		vk::ImageUsageFlags vkFlags{};

		if (HasFlag(_flags, TextureUsage::ShaderResource)) vkFlags |= vk::ImageUsageFlagBits::eSampled;
		if (HasFlag(_flags, TextureUsage::RenderTarget)) vkFlags |= vk::ImageUsageFlagBits::eColorAttachment;
		if (HasFlag(_flags, TextureUsage::DepthStencil)) vkFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
		if (HasFlag(_flags, TextureUsage::Storage)) vkFlags |= vk::ImageUsageFlagBits::eStorage;
		vkFlags |= vk::ImageUsageFlagBits::eTransferSrc;
		vkFlags |= vk::ImageUsageFlagBits::eTransferDst;

		return vkFlags;
	}

	vk::SamplerAddressMode ConvertToVkAddressMode(WrapMode _wrapMode)
	{
		switch (_wrapMode)
		{
		case WrapMode::Repeat:
			return vk::SamplerAddressMode::eRepeat;
		case WrapMode::ClampToEdge:
			return vk::SamplerAddressMode::eClampToEdge;
		case WrapMode::ClampToBorder:
			return vk::SamplerAddressMode::eClampToBorder;
		case WrapMode::MirrorRepeat:
			return vk::SamplerAddressMode::eMirroredRepeat;
		default:
			break;
		}
		return vk::SamplerAddressMode::eRepeat;
	}

	vk::Filter ConvertToVkFilter(FilterMode _filterMode)
	{
		switch (_filterMode)
		{
		case FilterMode::Nearest:
		case FilterMode::NearestMipmapNearest:
		case FilterMode::NearestMipmapLinear:
			return vk::Filter::eNearest;

		case FilterMode::Linear:
		case FilterMode::LinearMipmapNearest:
		case FilterMode::LinearMipmapLinear:
			return vk::Filter::eLinear;
		default:
			break;
		}
		return vk::Filter::eLinear;
	}

	vk::SamplerMipmapMode ConvertToVkMipmapMode(FilterMode _mipMapFilterMode)
	{
		switch (_mipMapFilterMode)
		{
		case FilterMode::Nearest:
		case FilterMode::NearestMipmapNearest:
		case FilterMode::LinearMipmapNearest:
			return vk::SamplerMipmapMode::eNearest;

		case FilterMode::Linear:
		case FilterMode::NearestMipmapLinear:
		case FilterMode::LinearMipmapLinear:
			return vk::SamplerMipmapMode::eLinear;
		default:
			break;
		}
		return vk::SamplerMipmapMode::eLinear;
	}

	vk::CompareOp ConvertToVkCompareOp(ComparisonFunc _func)
	{
		switch (_func)
		{
		case ComparisonFunc::Never:        return vk::CompareOp::eNever;
		case ComparisonFunc::Less:         return vk::CompareOp::eLess;
		case ComparisonFunc::Equal:        return vk::CompareOp::eEqual;
		case ComparisonFunc::LessEqual:    return vk::CompareOp::eLessOrEqual;
		case ComparisonFunc::Greater:      return vk::CompareOp::eGreater;
		case ComparisonFunc::NotEqual:     return vk::CompareOp::eNotEqual;
		case ComparisonFunc::GreaterEqual: return vk::CompareOp::eGreaterOrEqual;
		case ComparisonFunc::Always:       return vk::CompareOp::eAlways;
		default:
			break;
		}
		return vk::CompareOp::eNever;
	}

	vk::CullModeFlags ConvertToVulkanCullMode(const CullMode& _cullMode)
	{
		switch (_cullMode)
		{
		case CullMode::None:
			return vk::CullModeFlagBits::eNone;
		case CullMode::Front:
			return vk::CullModeFlagBits::eFront;
		case CullMode::Back:
			return vk::CullModeFlagBits::eBack;
		default:
			return vk::CullModeFlagBits::eNone;
		}
		return  vk::CullModeFlagBits::eFrontAndBack;
	}

	vk::PolygonMode ConvertToVulkanFillMode(const FillMode& _fillMode)
	{
		switch (_fillMode)
		{
		case FillMode::Solid:
			return vk::PolygonMode::eFill;
		case FillMode::Wireframe:
			return vk::PolygonMode::eLine;
		default:
			return vk::PolygonMode::eFill;
		}
		return vk::PolygonMode::eFill;
	}

	vk::SamplerCreateInfo TranslateToVulkanSamplerCreateInfo(const SamplerDesc& _desc)
	{
		vk::SamplerCreateInfo info{};

		info.magFilter = ConvertToVkFilter(_desc.magFilter);
		info.minFilter = ConvertToVkFilter(_desc.minFilter);
		info.mipmapMode = ConvertToVkMipmapMode(_desc.mipFilter);
		info.addressModeU = ConvertToVkAddressMode(_desc.wrapU);
		info.addressModeV = ConvertToVkAddressMode(_desc.wrapV);
		info.addressModeW = ConvertToVkAddressMode(_desc.wrapW);
		info.mipLodBias = 0.0f; // Optional
		info.anisotropyEnable = VK_FALSE;
		info.maxAnisotropy = _desc.maxAnisotropy;
		info.compareEnable = VK_FALSE;
		info.compareOp = ConvertToVkCompareOp(_desc.comparisonFunc);
		info.minLod = 0.0f; // Optional
		info.maxLod = VK_LOD_CLAMP_NONE;
		info.borderColor = vk::BorderColor::eIntTransparentBlack;
		info.unnormalizedCoordinates = VK_FALSE;

		return info;
	}


	vk::PipelineRasterizationStateCreateInfo TranslateToVulkanRasterizerCreateInfo(const RasterizerStateDesc& _desc)
	{
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = ConvertToVulkanFillMode(_desc.fillMode);
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = ConvertToVulkanCullMode(_desc.cullMode);
		rasterizer.frontFace = _desc.frontCounterClockwise ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		return rasterizer;
	}

}
