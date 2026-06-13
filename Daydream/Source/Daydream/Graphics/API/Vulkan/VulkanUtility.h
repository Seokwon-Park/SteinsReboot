#pragma once

#include "vulkan/vulkan.hpp"
#include "vk_mem_alloc.hpp"

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Sampler.h"
#include "Daydream/Graphics/States/RasterizerState.h"
#include "Daydream/Graphics/States/DepthStencilState.h"


namespace Daydream::GraphicsUtility::Vulkan
{
	Pair<vk::PipelineStageFlags, vk::AccessFlags> ConvertToVulkanStageAndAccess(ResourceState _state);
	vk::ImageLayout ConvertToVulkanImageLayout(ResourceState _state);
	vk::ImageAspectFlags GetImageAspectFlags(RenderFormat _format);

	vk::BufferCreateInfo ConvertToVkImageCreateInfo(const BufferDesc& _desc);
	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const BufferDesc& _desc);

	vk::ImageCreateInfo ConvertToVulkanCreateInfo(const TextureDesc& _desc);
	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const TextureDesc& _desc);

	vk::AttachmentLoadOp ConvertToLoadOp(AttachmentLoadOp op);
	vk::AttachmentStoreOp ConvertToStoreOp(AttachmentStoreOp op);

	vk::Format ConvertToVkFormat(RenderFormat _format);
	vk::ShaderStageFlagBits ConvertToShaderStageFlagBit(ShaderType _type);
	vk::ImageUsageFlags ConvertToVkImageUsageFlags(TextureUsage usageFlags);

	vk::SamplerAddressMode ConvertToVkAddressMode(WrapMode _wrapMode);
	vk::Filter ConvertToVkFilter(FilterMode _filter);
	vk::SamplerMipmapMode ConvertToVkMipmapMode(FilterMode _mipMapFilter);
	vk::CompareOp ConvertToVkCompareOp(ComparisonFunc _func);

	vk::CullModeFlags ConvertToVkCullMode(const CullMode& _cullMode);
	vk::PolygonMode ConvertToVkFillMode(const FillMode& _fillMode);

	vk::CompareOp ConvertToVkCompareOp(const CompareFunction& _compareFunc);
	vk::StencilOp ConvertToVkStencilOp(const StencilOperation& _stencilOp);
	vk::StencilOpState ConvertToVkStencilOpState(const StencilOperationDesc& _opDesc, UInt8 _readMask, UInt8 _writeMask);

	vk::SamplerCreateInfo TranslateToVkSamplerCreateInfo(const SamplerDesc& _desc);
	vk::PipelineRasterizationStateCreateInfo TranslateToVkRasterizationStateCreateInfo(const RasterizerStateDesc& _desc);
	vk::PipelineDepthStencilStateCreateInfo TranslateToVkDepthStencilStateCreateInfo(const DepthStencilStateDesc& _desc);

}